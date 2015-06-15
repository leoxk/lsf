// File:        proxysvrd.h
// Description: ---
// Notes:       ---
// Author:      leoxiang <leoxiang727@qq.com>
// Revision:    2015-06-10 by leoxiang

#include "lsf/basic/singleton.hpp"
#include "svr/common/basic_server.h"

class ProxyServer : 
    public BasicServer,
    public lsf::basic::Singleton<ProxyServer>
{
public:
    ProxyServer() : BasicServer(conf::SERVER_TYPE_PROXY_SERVER) { }

public:
    virtual bool OnInitProxy() { return true; }

    virtual bool OnInitNetLog() { return true; }

    virtual bool OnRun();

    virtual bool OnClientMessage();
};

// vim:ts=4:sw=4:et:ft=cpp:
