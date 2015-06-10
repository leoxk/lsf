// File:        tcpsvrd.h
// Description: ---
// Notes:       ---
// Author:      leoxiang <leoxiang727@qq.com>
// Revision:    2015-06-08 by leoxiang

#include "svr/common/basic_server.h"
#include "svr/common/common_func.h"

class TcpServer : 
    public BasicServer,
    public lsf::basic::Singleton<TcpServer>
{
public:
    TcpServer() : BasicServer(conf::SERVER_TYPE_TCP_SERVER) { }

public:
    virtual bool OnInitProxy() { return true; }

    virtual bool OnInitNetLog() { return true; }

    virtual bool OnRun();
};

// vim:ts=4:sw=4:et:ft=cpp:
