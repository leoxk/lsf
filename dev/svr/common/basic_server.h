// File:        basic_server.h
// Description: ---
// Notes:       ---
// Author:      leoxiang <leoxiang727@qq.com>
// Revision:    2015-06-08 by leoxiang

#pragma once

#include "lsf/asio/tcp.hpp"
#include "lsf/asio/net.hpp"
#include "lsf/basic/noncopyable.hpp"
#include "lsf/basic/error.hpp"
#include "svr/proto/conf_deploy.pb.h"

class BasicServer : public lsf::basic::NonCopyable {
public:
    BasicServer(conf::ENServerType server_type) : _server_type(server_type), _server_id(0) {}
    void Run(int argc, char** argv);

    // signal handle
    virtual void OnSignalHandle(int sig);

protected:
    conf::ConnectService const* GetConnectServiceConfig(conf::ENServiceType service_type);
    conf::AcceptService const* GetAcceptServiceConfig(conf::ENServiceType service_type);

protected:
    // init logic
    virtual bool OnParseCommond(int argc, char** argv);
    virtual bool OnInitDeployConfig();
    virtual bool OnSetCurrentPath(char const* command);
    virtual bool OnInitLocalLog();
    virtual bool OnDeamonize();
    virtual bool OnInitSignal();
    virtual bool OnRun() = 0;

    // other logic
    virtual void OnExit() {}
    virtual void OnTick() {}

public:
    conf::ENServerType GetServerType() const { return _server_type; }
    uint32_t GetServerId() const { return _server_id; }
    conf::Server const& GetServerConfig() const { return _server_config; }

    void SetServerConfig(conf::Server const& server_config) { _server_config.CopyFrom(server_config); }

protected:
    std::string _server_name;
    conf::ENServerType _server_type;
    uint32_t _server_id;
    std::string _confsvrd_addrss;
    conf::Server _server_config;
};

// vim:ts=4:sw=4:et:ft=cpp:
