// File:        test_server.h
// Description: ---
// Notes:       ---
// Author:      leoxiang <leoxiang727@qq.com>
// Revision:    2016-03-11 by leoxiang

#include "lsf/basic/singleton.hpp"
#include "svr/common/basic_server.h"

////////////////////////////////////////////////////////////
// TestServer
class TestServer : public BasicServer, public lsf::basic::Singleton<TestServer> {
public:
    TestServer() : BasicServer(conf::SERVER_TYPE_TEST_SERVER) {}

public:
    virtual bool OnRun();
};

// vim:ts=4:sw=4:et:ft=cpp:
