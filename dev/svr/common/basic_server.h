// File:        basic_server.h
// Description: ---
// Notes:       ---
// Author:      leoxiang <leoxiang727@qq.com>
// Revision:    2015-06-08 by leoxiang

#pragma once
#include "svr/common/common_header.h"
#include "svr/common/common_proto.h"

////////////////////////////////////////////////////////////
// BasicServer
class BasicServer : public lsf::basic::NonCopyable {
public:
    BasicServer(conf::ENServerType server_type) : _server_type(server_type) {}
    virtual ~BasicServer() {}
    void Run(int argc, char** argv);

    // signal handle
    virtual void OnSignalHandle(int sig);

protected:
    conf::ConnectService const* GetConnectServiceConfig(conf::ENServiceType service_type);
    conf::AcceptService const* GetAcceptServiceConfig(conf::ENServiceType service_type);

protected:
    // init logic
    virtual bool OnParseCommond();
    virtual bool OnInitConfigService();
    virtual bool OnInitCurrentPath();
    virtual bool OnInitLocalLog();
    virtual bool OnDeamonize();
    virtual bool OnInitSignal();
    virtual bool OnRun() = 0;

    // other logic
    virtual void OnExit() {}

public:
    conf::ENServerType GetServerType() const { return _server_type; }
    uint32_t GetServerId() const { return _server_id; }

protected:
    int                _argc;
    char**             _argv;
    std::string        _server_name;
    conf::ENServerType _server_type;
    uint32_t           _server_id = 0;
    std::string        _confsvrd_addrss;
};

// vim:ts=4:sw=4:et:ft=cpp:
