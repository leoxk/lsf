// File:        http_server.h
// Description: ---
// Notes:       ---
// Author:      leoxiang <leoxiang727@qq.com>
// Revision:    2015-09-04 by leoxiang

#include "lsf/basic/singleton.hpp"
#include "svr/common/basic_server.h"

////////////////////////////////////////////////////////////
// HttpServer
class HttpServer : public BasicServer, public lsf::basic::Singleton<HttpServer> {
public:
    HttpServer() : BasicServer(conf::SERVER_TYPE_HTTP_SERVER) {}

public:
    virtual bool OnRun();
    virtual void OnExit();
};

// vim:ts=4:sw=4:et:ft=cpp:
