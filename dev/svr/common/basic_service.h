// File:        basic_service.h
// Description: ---
// Notes:       ---
// Author:      leoxiang <leoxiang727@qq.com>
// Revision:    2015-06-11 by leoxiang

#pragma once

#include "lsf/asio/tcp.hpp"
#include "lsf/asio/io_service.hpp"
#include "lsf/basic/noncopyable.hpp"
#include "lsf/basic/error.hpp"
#include "svr/proto/conf_deploy.pb.h"

class BasicService : public lsf::basic::NonCopyable
{
public:
    BasicService(conf::ENServiceType service_type, conf::Service const & service_config,  lsf::asio::ProactorSerivce & io_service)
        : _service_type(service_type), _service_config(service_config), _io_service(io_service) { }

    bool Run();

protected:
    // connection callback
    virtual bool OnConnectionCreate(lsf::asio::tcp::Socket socket);

    virtual bool OnConnectionRead(lsf::asio::tcp::Socket socket);

    virtual bool OnConnectionPeerClose(lsf::asio::tcp::Socket socket);

private:
    // init logic
    bool OnInitListenSocket();

    bool OnInitConnectSocket();

    // async handler
    bool OnSocketAccept(lsf::asio::AsyncInfo & info);

    bool OnSocketConnect(lsf::asio::AsyncInfo & info);

    bool OnSocketRead(lsf::asio::AsyncInfo & info);

    bool OnSocketPeerClose(lsf::asio::AsyncInfo & info);


private:
    conf::ENServiceType             _service_type;
    conf::Service const &           _service_config;
    lsf::asio::ProactorSerivce &    _io_service;
};
                     
// vim:ts=4:sw=4:et:ft=cpp:
