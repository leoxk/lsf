// File:        protobuf_log.hpp
// Description: ---
// Notes:       ---
// Author:      leoxiang <leoxiang727@qq.com>
// Revision:    2014-09-30 by leoxiang

#pragma once

#include <vector>
#include "google/protobuf/stubs/common.h"
#include "lsf/basic/error.hpp"
#include "lsf/basic/singleton.hpp"
#include "lsf/basic/type_cast.hpp"

namespace lsf {
namespace util {

namespace detail {
static void ProtoBufLogHandler(google::protobuf::LogLevel level, char const * filename, int line, std::string const & message);
} // end of namespace detail

// ProtobufLog
class ProtobufLog : 
    public basic::Singleton<ProtobufLog>,
    public basic::Error
{
public:
    void Init() { google::protobuf::SetLogHandler(detail::ProtoBufLogHandler); }

    void LogHandle(google::protobuf::LogLevel level, char const * filename, int line, std::string const & message)
    {
        ErrString() = "[[" + 
                       lsf::basic::TypeCast<std::string>(level) + "|" +
                       filename + ":" + lsf::basic::TypeCast<std::string>(line) + "|" +
                       message +
                       "]]";
    }
};

namespace detail {
static void ProtoBufLogHandler(google::protobuf::LogLevel level, char const * filename, int line, std::string const & message)
{
    ProtobufLog::Instance()->LogHandle(level, filename, line, message);
}
} // end of namespace detail

} // end of namespace util
} // end of namespace lsf

// vim:ts=4:sw=4:et:
