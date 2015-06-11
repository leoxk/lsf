// File:        common_func.h
// Description: ---
// Notes:       ---
// Author:      leoxiang <leoxiang727@qq.com>
// Revision:    2015-06-08 by leoxiang

#pragma once

#include <string>
#include "lsf/util/protobuf_log.hpp"
#include "lsf/util/log.hpp"

class CommonFunc
{
////////////////////////////////////////////////////////////
// common func
public:


////////////////////////////////////////////////////////////
// template func
public:
    template<typename SocketType, typename MessageType>
    static bool SendAndRecv(SocketType & socket, MessageType & message)
    {
        // send request
        std::string content;
        if (!message.SerializeToString(&content))
        {
            LSF_LOG_ERR("%s", lsf::util::ProtobufLog::Instance()->ErrCharStr());
            return false;
        }
        if (!socket.SendAll(content))
        {
            LSF_LOG_ERR("size=%u, %s", content.size(), socket.ErrCharStr());
            return false;
        }

        // get response
        if (!socket.RecvAll(content))
        {
            LSF_LOG_ERR("%s", socket.ErrCharStr());
            return false;
        }

        // parse config
        if (!message.ParseFromString(content))
        {
            LSF_LOG_ERR("size=%u, %s", content.size(), lsf::util::ProtobufLog::Instance()->ErrCharStr());
            return false;
        }

        return true;
    }
};

// vim:ts=4:sw=4:et:ft=cpp:
