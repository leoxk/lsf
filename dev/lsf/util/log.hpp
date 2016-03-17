// File:        log.hpp
// Description: ---
// Notes:       ---
// Author:      leoxiang <leoxiang727@qq.com>
// Revision:    2015-09-08 by leoxiang

#pragma once
#include <initializer_list>
#include <iostream>
#include <fstream>
#include <memory>
#include "lsf/basic/macro.hpp"
#include "lsf/basic/singleton.hpp"
#include "lsf/util/string_ext.hpp"
#include "lsf/util/system.hpp"
#include "lsf/util/date.hpp"
#include "lsf/util/backtrace.hpp"
#include "lsf/util/protobuf.hpp"
#include "lsf/util/log.hpp"

namespace lsf {
namespace util {

////////////////////////////////////////////////////////////
// LogFileBuf
class LogFileBuf : public std::streambuf {
public:
    const static int SHIFT_NONE = 0;
    const static int SHIFT_DAY = 1;
    const static int SHIFT_MONTH = 2;
    static const char EOL = '\n';

public:
    LogFileBuf(std::string const& file_prefix, int shift_type) :
        _shift_type(shift_type), _file_prefix(file_prefix) {
            Open();
            if (IsOpen()) TraverseAndMove();
        }

    virtual int overflow(int c) override {
        if (c == EOL) {
            _file_buf.pubsync();
            if (Shift()) Open();
        }
        return _file_buf.sputc(c);
    }

    bool IsOpen() const { return _file_buf.is_open(); }

protected:
    void Open() {
        // save last
        lsf::util::Date date;
        switch (_shift_type) {
            case SHIFT_NONE:  break;
            case SHIFT_DAY:   _last_shift_val = date.GetDay();   break;
            case SHIFT_MONTH: _last_shift_val = date.GetMonth(); break;
            default: return;
        }

        // construct path
        switch (_shift_type) {
            case SHIFT_NONE:    _file_path = _file_prefix; break;
            case SHIFT_DAY:
            case SHIFT_MONTH:
            default:            _file_path = _file_prefix + date.ToFormatString(".%Y-%m-%d"); break;
        }

        // mkdir if necessary
        auto file_dir = lsf::util::StringExt::GetDirName(_file_path);
        if (!lsf::util::System::IsExist(file_dir) && !lsf::util::System::MkDir(file_dir)) return;

        // open file
        if (_file_buf.is_open()) _file_buf.close();
        _file_buf.open(_file_path.c_str(), std::ios::app | std::ios::out);
    }

    bool Shift() {
        // check file exist
        if (!IsOpen()) return false;

        // check need shift
        switch (_shift_type) {
            case SHIFT_DAY:   if (_last_shift_val == (int)lsf::util::Date().GetDay())   return false; break;
            case SHIFT_MONTH: if (_last_shift_val == (int)lsf::util::Date().GetMonth()) return false; break;
            default: return false;
        }

        // real shift
        if (!RealShift(_file_path)) return false;

        // close old file
        _file_buf.close();
        return true;
    }

    bool RealShift(std::string const& file) {
        // check head match
        if (::strncmp(file.c_str(), _file_prefix.c_str(), _file_prefix.size()) != 0) return false;

        // scanf date
        unsigned int year, month, day;
        if (sscanf(file.c_str() + _file_prefix.size(), ".%u-%u-%u", &year, &month, &day) == EOF) return false;

        // check date match
        lsf::util::Date date;
        if (year == date.GetYear() && month == date.GetMonth() && day == date.GetDay()) return false;

        // calc new file path
        char const* nest_dir;
        switch (_shift_type) {
            case SHIFT_DAY:   nest_dir = "/%Y/%m/"; break;
            case SHIFT_MONTH: nest_dir = "/%Y/";    break;
            default: return false;
        }
        std::string new_file_path = lsf::util::StringExt::GetDirName(file) +
                        date.ToFormatString(nest_dir) +
                        lsf::util::StringExt::GetBaseName(file);

        // mkdir and rename
        auto new_file_dir = lsf::util::StringExt::GetDirName(new_file_path);
        if (!lsf::util::System::IsExist(new_file_dir) && !lsf::util::System::MkDir(new_file_dir)) return false;
        if (!lsf::util::System::Rename(file, new_file_path)) return false;

        return true;
    }

    void TraverseAndMove() {
        // traverse all file
        lsf::util::System::ForFilesWithinFold(
                lsf::util::StringExt::GetDirName(_file_prefix),
                [this] (std::string const& file) { RealShift(file); });
    }

protected:
    int _shift_type = SHIFT_NONE;
    int _last_shift_val = -1;
    std::string _file_prefix;
    std::string _file_path;
    std::filebuf _file_buf;
};

////////////////////////////////////////////////////////////
// Log
class Log : public std::ostream {
public:
    const static int TYPE_INF = 0x1 << 0;
    const static int TYPE_DBG = 0x1 << 1;
    const static int TYPE_WRN = 0x1 << 2;
    const static int TYPE_ERR = 0x1 << 3;
    const static int TYPE_FAT = 0x1 << 4;
    const static int TYPE_ALL = TYPE_INF | TYPE_DBG | TYPE_WRN | TYPE_ERR | TYPE_FAT;
    const static size_t MAX_LOG_SIZE = 32768;
    using base_type = std::ostream;

public:
    ////////////////////////////////////////////////////////////
    // common func
    Log(int mask = TYPE_ALL) : base_type(std::cerr.rdbuf()), _mask(mask) {}
    ~Log() { Release(); }


    void BingTerminalOut() { base_type::rdbuf(std::cout.rdbuf()); }
    void BingTerminalErr() { base_type::rdbuf(std::cerr.rdbuf()); }
    void BindString() { Reset(new std::stringbuf()); }

    bool BindFile(std::string const& file_prefix, int shift_type = LogFileBuf::SHIFT_NONE) {
        auto ptmp = new LogFileBuf(file_prefix, shift_type);
        if (!ptmp->IsOpen()) {
            delete ptmp;
            return false;
        }

        Reset(ptmp);
        return true;
    }

    std::string GetString() const {
        if (std::stringbuf* psb = dynamic_cast<std::stringbuf*>(base_type::rdbuf())) {
            return psb->str();
        }
        return "";
    }

    bool CheckMask(int type) const { return type & _mask; }

    ////////////////////////////////////////////////////////////
    // output func
    Log& Output(int type, char const* fmt, ...) {
        // check input
        if (!base_type::good()) return *this;
        if (!CheckMask(type)) return *this;
        if (fmt == nullptr) return *this;

        // get user-define content
        char tmp[MAX_LOG_SIZE];
        va_list ap;
        va_start(ap, fmt);
        vsnprintf(tmp, sizeof(tmp), fmt, ap);
        va_end(ap);

        // do ouotput
        base_type::write(tmp, strnlen(tmp, sizeof(tmp)));
        base_type::flush();

        return *this;
    }

protected:
    void Release() {
        if (_psb) delete _psb;
    }

    void Reset(std::streambuf* psb) {
        Release();
        _psb = psb;
        base_type::rdbuf(_psb);
    }

    int _mask = TYPE_ALL;
    std::streambuf* _psb = nullptr;
};

////////////////////////////////////////////////////////////
// SingleLog
class SingleLog : public Log, public lsf::basic::Singleton<SingleLog> { };

} // end namespace util
} // end namespace lsf

////////////////////////////////////////////////////////////
// macros
#define LSF_LOG             lsf::util::SingleLog::Reference()
#define LSF_DATE            lsf::util::Date::Now().ToFormatString("%Y-%m-%d|%H:%M:%S")
#define LSF_STACK(count)    lsf::util::Backtrace::Instance()->ToString(5).c_str()
#define LSF_PROTOBUF        lsf::util::Protobuf::Instance()->ErrString().c_str()

#define LSF_LOG_INF(fmt, ...) LSF_LOG.Output(lsf::util::Log::TYPE_INF, "%s|INF " __FILE__ "|" LSF_TOKEN_TO_STRING(__LINE__) " " fmt "\n", LSF_DATE.c_str(), ##__VA_ARGS__)
#define LSF_LOG_DBG(fmt, ...) LSF_LOG.Output(lsf::util::Log::TYPE_DBG, "%s|DBG " __FILE__ "|" LSF_TOKEN_TO_STRING(__LINE__) " " fmt "\n", LSF_DATE.c_str(), ##__VA_ARGS__)
#define LSF_LOG_WRN(fmt, ...) LSF_LOG.Output(lsf::util::Log::TYPE_WRN, "%s|WRN " __FILE__ "|" LSF_TOKEN_TO_STRING(__LINE__) " " fmt "\n", LSF_DATE.c_str(), ##__VA_ARGS__)
#define LSF_LOG_ERR(fmt, ...) LSF_LOG.Output(lsf::util::Log::TYPE_ERR, "%s|ERR " __FILE__ "|" LSF_TOKEN_TO_STRING(__LINE__) " " fmt "\n", LSF_DATE.c_str(), ##__VA_ARGS__)
#define LSF_LOG_FAT(fmt, ...) LSF_LOG.Output(lsf::util::Log::TYPE_FAT, "%s|FAT " __FILE__ "|" LSF_TOKEN_TO_STRING(__LINE__) " " fmt "\n", LSF_DATE.c_str(), ##__VA_ARGS__)

#define LSF_LOG_INF_STACK(fmt, ...) LSF_LOG_INF(fmt "\n%s", ##__VA_ARGS__, LSF_STACK(5))
#define LSF_LOG_DBG_STACK(fmt, ...) LSF_LOG_DBG(fmt "\n%s", ##__VA_ARGS__, LSF_STACK(5))
#define LSF_LOG_WRN_STACK(fmt, ...) LSF_LOG_WRN(fmt "\n%s", ##__VA_ARGS__, LSF_STACK(5))
#define LSF_LOG_ERR_STACK(fmt, ...) LSF_LOG_ERR(fmt "\n%s", ##__VA_ARGS__, LSF_STACK(5))
#define LSF_LOG_FAT_STACK(fmt, ...) LSF_LOG_FAT(fmt "\n%s", ##__VA_ARGS__, LSF_STACK(5))

#define LSF_INF if (LSF_LOG.CheckMask(lsf::util::Log::TYPE_INF)) LSF_LOG << LSF_DATE << "|INF " __FILE__ "|" LSF_TOKEN_TO_STRING(__LINE__) " "
#define LSF_DBG if (LSF_LOG.CheckMask(lsf::util::Log::TYPE_DBG)) LSF_LOG << LSF_DATE << "|DBG " __FILE__ "|" LSF_TOKEN_TO_STRING(__LINE__) " "
#define LSF_WRN if (LSF_LOG.CheckMask(lsf::util::Log::TYPE_WRN)) LSF_LOG << LSF_DATE << "|WRN " __FILE__ "|" LSF_TOKEN_TO_STRING(__LINE__) " "
#define LSF_ERR if (LSF_LOG.CheckMask(lsf::util::Log::TYPE_ERR)) LSF_LOG << LSF_DATE << "|ERR " __FILE__ "|" LSF_TOKEN_TO_STRING(__LINE__) " "
#define LSF_FAT if (LSF_LOG.CheckMask(lsf::util::Log::TYPE_FAT)) LSF_LOG << LSF_DATE << "|FAT " __FILE__ "|" LSF_TOKEN_TO_STRING(__LINE__) " "

// vim:ts=4:sw=4:et:ft=cpp:
