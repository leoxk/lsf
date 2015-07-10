// File:        room_server.cpp
// Description: ---
// Notes:       ---
// Author:      leoxiang <leoxiang727@qq.com>
// Revision:    2015-07-07 by leoxiang

#include "svr/common/common_header.h"
#include "svr/common/common_service.h"
#include "svr/common/session_manager.h"
#include "svr/common/timer_manager.h"
#include "svr/common/handler_manager.h"
#include "svr/roomsvrd/room_server.h"
#include "svr/roomsvrd/handler_common.h"

using namespace google::protobuf;
using namespace lsf::util;

bool RoomServer::OnRun() {
    // init session manager
    if (!SessionManager::Instance()->Init(
                _server_config.session_key(),
                _server_config.session_size())) return false;

    // init timer manager
    if (!TimerManager::Instance()->Init(
                _server_config.timer_key(),
                _server_config.timer_size())) return false;

    // init service
    if (!ConnectClientMsgTransferService::Instance()->Run(this)) return false;
    if (!ConnectServerMsgTransferService::Instance()->Run(this)) return false;

    // register handler
    HandlerManager::Instance()->AddHandler(msg::CS::kQuickStartReqFieldNumber, new QuickStartHandler());

    return true;
}

int main(int argc, char** argv) { RoomServer::Instance()->Run(argc, argv); }

// vim:ts=4:sw=4:et:ft=cpp:
