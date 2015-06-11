// File:        basic_server.h
// Description: ---
// Notes:       ---
// Author:      leoxiang <leoxiang727@qq.com>
// Revision:    2015-06-08 by leoxiang

#pragma once

#include "lsf/asio/tcp.hpp"
#include "lsf/asio/io_service.hpp"
#include "lsf/basic/noncopyable.hpp"
#include "lsf/basic/error.hpp"
#include "svr/proto/conf_deploy.pb.h"

class BasicServer : public lsf::basic::NonCopyable
{
public:
    BasicServer(conf::ENServerType server_type, lsf::asio::ProactorSerivce & io_service) : 
        _io_service(io_service), _server_type(server_type), _server_id(0) { }

    void Run(int argc, char** argv);

protected:
    // init logic
    virtual bool OnParseCommond(int argc, char** argv);

    virtual bool OnGetConfig();

    virtual bool OnSetCurrentPath(char const * command);

    virtual bool OnInitLocalLog();

    virtual bool OnDeamonize();

    virtual bool OnRun() = 0;

protected:
    lsf::asio::ProactorSerivce & _io_service;
    std::string                  _server_name;
    conf::ENServerType           _server_type;
    uint32_t                     _server_id;
    lsf::asio::tcp::SockAddr     _confsvrd_addrss;
    conf::Server                 _server_config;
};

// vim:ts=4:sw=4:et:ft=cpp:
