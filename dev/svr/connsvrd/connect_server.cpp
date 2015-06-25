// File:        connect_server.cpp
// Description: ---
// Notes:       ---
// Author:      leoxiang <leoxiang727@qq.com>
// Revision:    2015-06-22 by leoxiang

#include "svr/common/common_header.h"
#include "svr/connsvrd/connect_server.h"
#include "svr/connsvrd/client_msg_service.h"
#include "svr/connsvrd/cmsg_transfer_service.h"

using namespace google::protobuf;
using namespace lsf::util;
using namespace lsf::asio;

bool ConnectServer::OnRun() {
    // init service
    if (!AcceptClientMsgService::Instance()->Run(this)) return false;
    if (!AcceptClientMsgTransferService::Instance()->Run(this)) return false;
    return true;
}

int main(int argc, char** argv) {
    // use epoll high concurrency
    IOService::UseEpoll();
    ConnectServer::Instance()->Run(argc, argv);
}

// vim:ts=4:sw=4:et:ft=cpp:
