// File:        benchmark.cpp
// Description: ---
// Notes:       ---
// Author:      leoxiang <leoxiang727@qq.com>
// Revision:    2016-03-11 by leoxiang

#include "svr/common/common_header.h"
#include "svr/common/common_service.h"
#include "svr/testsvrd/test_server.h"
#include "svr/testsvrd/connect_client_service.h"

using namespace google::protobuf;
using namespace lsf::util;
using namespace lsf::asio;

bool TestServer::OnRun() {
    // init service
    for (int i = 0; i < 10000; ++i) {
        auto* pservice = new ConnectClientService(10000);
        if (!pservice->Run(this)) return false;
    }

    return true;
}

int main(int argc, char** argv) {
    // use epoll high concurrency
    IOService::UseEpoll();
    TestServer::Instance()->Run(argc, argv);
    LSF_LOG_INF("exit");
}

// LSF_TEST_CASE(test_begin) {
// }
//
// int main(int argc, char **argv) { LSF_TEST_ALL(argc, argv); }

// vim:ts=4:sw=4:et:ft=cpp:
