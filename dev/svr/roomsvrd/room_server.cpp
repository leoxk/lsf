// File:        room_server.cpp
// Description: ---
// Notes:       ---
// Author:      leoxiang <leoxiang727@qq.com>
// Revision:    2015-07-07 by leoxiang

#include "svr/common/common_header.h"
#include "svr/common/common_service.h"
#include "svr/roomsvrd/room_server.h"

using namespace google::protobuf;
using namespace lsf::util;

bool RoomServer::OnRun() {
    // init service
    if (!ConnectClientMsgTransferService::Instance()->Run(this)) return false;
    if (!ConnectServerMsgTransferService::Instance()->Run(this)) return false;
    return true;
}

int main(int argc, char** argv) { RoomServer::Instance()->Run(argc, argv); }

// vim:ts=4:sw=4:et:ft=cpp:
