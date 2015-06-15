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
#include "svr/common/basic_server.h"

class BasicService : public lsf::basic::NonCopyable
{
public:
    BasicService(conf::ENServiceType service_type) : _service_type(service_type){ }

    bool Run(BasicServer * pserver);

public:
    // async handler
    virtual bool OnSocketAccept(lsf::asio::AsyncInfo & info);

    virtual bool OnSocketConnect(lsf::asio::AsyncInfo & info);

    virtual bool OnSocketRead(lsf::asio::AsyncInfo & info);

    virtual bool OnSocketPeerClose(lsf::asio::AsyncInfo & info);

protected:
    // init logic
    bool OnInitConfig();

    bool OnInitListenSocket();

    bool OnInitConnectSocket();

private:
    conf::ENServiceType             _service_type;
    conf::Service                   _service_config;
    BasicServer *                   _pserver;
};
                     
// vim:ts=4:sw=4:et:ft=cpp:
