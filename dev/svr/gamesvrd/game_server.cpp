// File:        game_server.cpp
// Description: ---
// Notes:       ---
// Author:      leoxiang <leoxiang727@qq.com>
// Revision:    2015-06-25 by leoxiang

#include "svr/common/common_header.h"
#include "svr/common/common_service.h"
#include "svr/gamesvrd/game_server.h"
#include "svr/common/session_manager.h"
#include "svr/common/timer_manager.h"

using namespace google::protobuf;
using namespace lsf::util;

bool GameServer::OnRun() {
    // init session manager
    if (!SessionManager::Instance()->Init(_server_config.session_key(), _server_config.session_size())) return false;

    // init timer manager
    if (!TimerManager::Instance()->Init(_server_config.timer_key(), _server_config.timer_size())) return false;

    // init service
    if (!ConnectClientMsgTransferService::Instance()->Run(this)) return false;
    if (!ConnectServerMsgTransferService::Instance()->Run(this)) return false;

    return true;
}

void GameServer::OnExit() {
    // serialize container
    SessionManager::Instance()->Release();
    TimerManager::Instance()->Release();
}

void GameServer::OnTick() {
    // session check
    SessionManager::Instance()->CheckLeak();

    // timer tick
    TimerManager::Instance()->Tick();
}

int main(int argc, char** argv) { GameServer::Instance()->Run(argc, argv); }

// vim:ts=4:sw=4:et:ft=cpp:
