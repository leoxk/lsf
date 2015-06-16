// File:        basic_accept_service.h
// Description: ---
// Notes:       ---
// Author:      leoxiang <leoxiang727@qq.com>
// Revision:    2015-06-11 by leoxiang

#pragma once

#include <set>
#include "lsf/asio/tcp.hpp"
#include "lsf/asio/io_service.hpp"
#include "lsf/basic/noncopyable.hpp"
#include "lsf/basic/error.hpp"
#include "svr/proto/conf_deploy.pb.h"
#include "svr/common/basic_server.h"

////////////////////////////////////////////////////////////
// BasicAcceptService
class BasicAcceptService : public lsf::basic::NonCopyable
{
public:
    BasicAcceptService(conf::ENServiceType service_type) : _service_type(service_type){ }
    bool Run(BasicServer * pserver);

public:
    // connection handler
    virtual bool OnConnectionCreate(lsf::asio::Socket socket, lsf::asio::Socket listen_socket);
    virtual bool OnConnectionMessage(lsf::asio::Socket socket, std::string & buffer);
    virtual bool OnConnectionPeerClose(lsf::asio::Socket socket);

protected:
    // init logic
    virtual bool OnInitConfig();
    virtual bool OnInitSocket();

    // async handler
    virtual bool OnSocketAccept(lsf::asio::AsyncInfo & info);
    virtual bool OnSocketRead(lsf::asio::AsyncInfo & info);
    virtual bool OnSocketPeerClose(lsf::asio::AsyncInfo & info);

protected:
    conf::ENServiceType             _service_type;
    conf::AcceptService             _service_config;
    BasicServer *                   _pserver;
};
                     
// vim:ts=4:sw=4:et:ft=cpp:
