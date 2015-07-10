// File:        proxy_server.h
// Description: ---
// Notes:       ---
// Author:      leoxiang <leoxiang727@qq.com>
// Revision:    2015-06-10 by leoxiang

#include "lsf/basic/singleton.hpp"
#include "svr/common/basic_server.h"

////////////////////////////////////////////////////////////
// ProxyServer
class ProxyServer : public BasicServer, public lsf::basic::Singleton<ProxyServer> {
public:
    ProxyServer() : BasicServer(conf::SERVER_TYPE_PROXY_SERVER) {}

public:
    virtual bool OnRun();
};

// vim:ts=4:sw=4:et:ft=cpp:
