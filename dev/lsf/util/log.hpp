// File:        log.hpp
// Description: ---
// Notes:       
// Author:      leoxiang <leoxiang727@qq.com>
// Revision:    2011-09-19 by leoxiang

#pragma once

#include <stdint.h>
#include <stdarg.h>
#include <iostream>
#include <fstream>
#include <cstring>
#include "lsf/util/system.hpp"
#include "lsf/util/date.hpp"
#include "lsf/basic/singleton.hpp"
#include "lsf/basic/error.hpp"

namespace lsf {
namespace util {

////////////////////////////////////////////////////////////
// FileLogDriver
////////////////////////////////////////////////////////////
class FileLogDriver : public basic::Error
{
public:
    const static uint16_t SHIFT_NONE  = 0;
    const static uint16_t SHIFT_DAY   = 1;
    const static uint16_t SHIFT_MONTH = 2;
    
public:
    FileLogDriver() : _shift(), _last_shift(0)
    { }

    FileLogDriver(std::string const & prefix, uint16_t shift = SHIFT_DAY)
        : _shift(shift), 
          _prefix(prefix)
    { _Open(); }

    FileLogDriver(FileLogDriver const & rhs)
        : _shift(rhs._shift),
          _prefix(rhs._prefix)
    { _Open(); }

    FileLogDriver & operator=(FileLogDriver const & rhs) {
        if (this == &rhs) return *this;
        _Shift();
        _shift  = rhs._shift;
        _prefix = rhs._prefix;
        _Open();
        return *this;
    }

    ~FileLogDriver() {
        _Shift();
    }

    // member funcs
    size_t Write(char const * str, size_t len) {
        if (_Shift()) _Open();
        
        _ofs.write(str, len);
        return len;
    }

    void Flush() { if (!IsReady()) return; _ofs.flush(); }

    bool IsReady() const { return _ofs.is_open(); } 

private:
    bool _Open()
    {
        if (_ofs.is_open()) _ofs.close();

        // mkdir if necessary
        if (!System::IsExist(basic::StringExt::GetDirName(_prefix))) {
            if (!System::MkDir(basic::StringExt::GetDirName(_prefix))) {
                ErrString() = std::string("System::Mkdir: ") + System::ErrString();
                return false;
            }
        }

        // construct file path
        if (SHIFT_DAY == _shift) {
            _last_shift = Date().GetDay();
            _log_path = _prefix + "." + Date().ToFormatString("%Y-%m-%d");
        }
        else if (SHIFT_MONTH == _shift) {
            _last_shift = Date().GetMonth();
            _log_path = _prefix + "." + Date().ToFormatString("%Y-%m");
        }
        else {
            _log_path = _prefix; 
        }

        // open file
        _ofs.open(_log_path.c_str(), std::ios_base::app | std::ios_base::out);

        if (!_ofs.is_open()) {
            ErrString() = std::string("ofstream::open: ") + SysErrString();
            return false;
        }
        return true;
    }

    bool _Shift()
    {
        if (_shift == SHIFT_NONE) return false;
        if (_log_path.empty()) return false;

        // construct path
        std::string   dest_path;
        if (SHIFT_DAY == _shift)
        {
            if (Date().GetDay() == _last_shift)  return false;
            dest_path = basic::StringExt::GetDirName(_log_path) + Date().ToFormatString("/%Y/%m/") + 
                basic::StringExt::GetBaseName(_log_path);
        }
        else if (SHIFT_MONTH == _shift)
        {
            if (Date().GetMonth() == _last_shift) return false;
            dest_path = basic::StringExt::GetDirName(_log_path) + Date().ToFormatString("/%Y/") + 
                basic::StringExt::GetBaseName(_log_path);
        }
        else 
        {
            return false;
        }

        // mkdir and rename
        if (!System::IsExist(basic::StringExt::GetDirName(dest_path)))
        {
            if (!System::MkDir(basic::StringExt::GetDirName(dest_path))) {
                ErrString() = std::string("System::Mkdir: ") + System::ErrString();
                return false;
            }
        }

        if (!System::Rename(_log_path, dest_path))
        {
            ErrString() = std::string("System::Rename: ") +  System::ErrString();
            return false;
        }

        // close file
        if (_ofs.is_open()) _ofs.close();

        return true;
    }

private:
    uint16_t            _shift;
    size_t              _last_shift;
    std::string         _prefix;
    std::string         _log_path;
    std::ofstream       _ofs;
};

////////////////////////////////////////////////////////////
// Log
////////////////////////////////////////////////////////////
template<typename DriverType>
class BasicLog : public basic::Error
{
public:
    // constants
    const static uint16_t TYPE_INFO  = 0x1 << 0;
    const static uint16_t TYPE_DEBUG = 0x1 << 1;
    const static uint16_t TYPE_WARN  = 0x1 << 2;
    const static uint16_t TYPE_ERR   = 0x1 << 3;
    const static uint16_t TYPE_FATAL = 0x1 << 4;
    const static uint16_t TYPE_ALL   = TYPE_INFO | TYPE_DEBUG | TYPE_WARN | TYPE_ERR | TYPE_FATAL;

public:
    // constructor
    explicit BasicLog(uint16_t log_mask = TYPE_ALL) 
        : _log_mask(log_mask) { }

    // membet funcs
    bool BindOutput(DriverType const & driver) {
        _driver = driver;
        
        if (!_driver.IsReady())
        {
            ErrString() = std::string("Driver::IsReady() : ") + _driver.ErrString();
            return false;
        }
        return true;
    }

    size_t WriteLog(uint16_t type, char const * fmt, ...) {
        if (!IsBindOuput()) return 0;
        if (!(type & _log_mask)) return 0;
        if (fmt == NULL) return 0;

        // get time prefix
        std::string prefix = Date::Now().ToFormatString("[%Y-%m-%d %H:%M:%S ");
        switch (type)
        {
            case TYPE_INFO:     prefix += "INF] "; break;
            case TYPE_DEBUG:    prefix += "DBG] "; break;
            case TYPE_WARN:     prefix += "WRN] "; break;
            case TYPE_ERR:      prefix += "ERR] "; break;
            case TYPE_FATAL:    prefix += "FAT] "; break;
            default:            prefix += "UKN] "; break;
        }

        // get user-define content
        char tmp[1024];
        va_list ap;
        va_start(ap, fmt);
        vsprintf(tmp, fmt, ap);
        va_end(ap);

        // do ouotput
        size_t len = 0;
        len += _driver.Write(prefix.c_str(), prefix.size());
        len += _driver.Write(tmp, strlen(tmp));
        len += _driver.Write("\n", 1);
        _driver.Flush();

        return len;
    }
    
    // accessor
    void SetTypeMask(uint16_t type) { _log_mask = type; }

    bool IsBindOuput() const { return _driver.IsReady(); }

private:
    DriverType      _driver;
    uint16_t        _log_mask;
}; 

////////////////////////////////////////////////////////////
// provide macro for convient access
typedef BasicLog<FileLogDriver>     FileLog;

////////////////////////////////////////////////////////////
// Singleton Log, provide macros for convient access
class SingleLog : 
    public FileLog, 
    public basic::Singleton<SingleLog>
{ };

#define LSF_LOG_INFO(fmt, args...)  lsf::util::SingleLog::Instance()->WriteLog(\
        ::lsf::util::FileLog::TYPE_INFO,  "%s|%s|%d " fmt, __FILE__, __FUNCTION__, __LINE__, ##args)

#define LSF_LOG_DEBUG(fmt, args...) lsf::util::SingleLog::Instance()->WriteLog(\
        ::lsf::util::FileLog::TYPE_DEBUG, "%s|%s|%d " fmt, __FILE__, __FUNCTION__, __LINE__, ##args)

#define LSF_LOG_WARN(fmt, args...)  lsf::util::SingleLog::Instance()->WriteLog(\
        ::lsf::util::FileLog::TYPE_WARN,  "%s|%s|%d " fmt, __FILE__, __FUNCTION__, __LINE__, ##args)

#define LSF_LOG_ERR(fmt, args...)   lsf::util::SingleLog::Instance()->WriteLog(\
        ::lsf::util::FileLog::TYPE_ERR,   "%s|%s|%d " fmt, __FILE__, __FUNCTION__, __LINE__, ##args)

#define LSF_LOG_FATAL(fmt, args...) lsf::util::SingleLog::Instance()->WriteLog(\
        ::lsf::util::FileLog::TYPE_FATAL, "%s|%s|%d " fmt, __FILE__, __FUNCTION__, __LINE__, ##args)


} // end of namespace util
} // end of namespace lsf

// vim:ts=4:sw=4:et:ft=cpp:
