// File:        common_func.h
// Description: ---
// Notes:       ---
// Author:      leoxiang <leoxiang727@qq.com>
// Revision:    2015-06-08 by leoxiang

#pragma once

#include <string>
#include "lsf/util/protobuf_log.hpp"
#include "lsf/util/backtrace.hpp"
#include "lsf/util/log.hpp"
#include "lsf/asio/tcp.hpp"
#include "svr/proto/conf_deploy.pb.h"

class CommonFunc
{
////////////////////////////////////////////////////////////
// common func
public:
    ////////////////////////////////////////////////////////////
    // pack and unpack
    static bool PackMsg(std::string & content, google::protobuf::MessageLite const & message);

    static bool UnPackMsg(std::string const & content, google::protobuf::MessageLite & message);

    ////////////////////////////////////////////////////////////
    // send and recv
    static bool SendAll(lsf::asio::tcp::Socket socket, std::string const & content);

    static bool RecvAll(lsf::asio::tcp::Socket socket, std::string & content);

////////////////////////////////////////////////////////////
// template func
public:
    template<typename SocketType, typename MessageType>
    static bool SendAndRecv(SocketType & socket, MessageType & message)
    {
        // send request
        std::string content;
        if (!PackMsg(content, message)) return false;
        if (!SendAll(socket, content)) return false;

        // get response
        if (!RecvAll(socket, content)) return false;


        // parse config
        if (!UnPackMsg(content, message)) return false;

        return true;
    }
};

// vim:ts=4:sw=4:et:ft=cpp:
