// File:        protobuf_log.hpp
// Description: ---
// Notes:       ---
// Author:      leoxiang <xiangkun@ximigame.com>
// Revision:    2014-09-30 by leoxiang
//
// Copyright (C) 1998-2014 XiMi. All Rights Reserved.
// This is unpublished proprietary source code of XiMi Ltd. The copyright
// notice above does not evidence any actual or intended publication of such
// source code. UNAUTHORIZED DISTRIBUTION, ADAPTATION OR USE MAY BE SUBJECT
// TO CIVIL AND CRIMINAL PENALTIES.

#pragma once

#include <vector>
#include "google/protobuf/stubs/common.h"
#include "lsf/basic/singleton.hpp"
#include "lsf/basic/type_cast.hpp"

namespace lsf {
namespace util {

namespace detail {
static void ProtoBufLogHandler(google::protobuf::LogLevel level, char const * filename, int line, std::string const & message);
} // end of namespace detail

// ProtobufLog
class ProtobufLog : public lsf::basic::Singleton<ProtobufLog>
{
public:
    ProtobufLog() : _need_clear(false) { }

    void Init() { google::protobuf::SetLogHandler(detail::ProtoBufLogHandler); }

    void LogHandle(google::protobuf::LogLevel level, char const * filename, int line, std::string const & message)
    {
        _CheckClear();
        _err_string += "[[" + 
                       lsf::basic::TypeCast<std::string>(level) + "|" +
                       filename + ":" + lsf::basic::TypeCast<std::string>(line) + "|" +
                       message +
                       "]]";
    }

    std::string GetErrString() { 
        _CheckClear();
        return _err_string; 
    }

    std::string PopErrString() { 
        _CheckClear();
        _need_clear = true; return _err_string; 
    }

private:
    void _CheckClear()
    {
        if (_need_clear) {
            _need_clear = false;
            _err_string.clear();
        }
    }

private:
    bool           _need_clear;
    std::string    _err_string;
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
