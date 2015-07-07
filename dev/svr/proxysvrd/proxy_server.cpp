// File:        proxy_server.cpp
// Description: ---
// Notes:       ---
// Author:      leoxiang <leoxiang727@qq.com>
// Revision:    2015-06-10 by leoxiang

#include "svr/common/common_header.h"
#include "svr/proxysvrd/proxy_server.h"
#include "svr/proxysvrd/server_msg_transfer_service.h"

using namespace google::protobuf;
using namespace lsf::util;

bool ProxyServer::OnRun() {
    // init service
    if (!AcceptServerMsgTransferService::Instance()->Run(this)) return false;
    if (!AcceptServerMsgTransferService::Instance()->InitSocketMapFromServerConfig()) return false;
    return true;
}

int main(int argc, char** argv) { ProxyServer::Instance()->Run(argc, argv); }

// vim:ts=4:sw=4:et:ft=cpp:
