// File:        game_server.cpp
// Description: ---
// Notes:       ---
// Author:      leoxiang <leoxiang727@qq.com>
// Revision:    2015-06-25 by leoxiang

#include "lsf/util/words_filter.hpp"
#include "svr/common/common_header.h"
#include "svr/common/common_service.h"
#include "svr/common/session_manager.h"
#include "svr/common/timer_manager.h"
#include "svr/common/config_manager.h"
#include "svr/common/handler_manager.h"
#include "svr/common/client_conn_manager.h"
#include "svr/gamesvrd/game_server.h"
#include "svr/gamesvrd/table_manager.h"
#include "svr/gamesvrd/handler_game.h"

using namespace google::protobuf;
using namespace lsf::util;

bool GameServer::OnRun() {
    // init service
    if (!ConnectClientMsgTransferService::Instance()->Run(this)) return false;
    if (!ConnectServerMsgTransferService::Instance()->Run(this)) return false;
    ConnectLogService::Instance()->Run(this);

    // init session manager
    if (!SessionManager::Instance()->Init(
                ConfigManager::Instance()->ServerConfig().session_key(),
                ConfigManager::Instance()->ServerConfig().session_size())) return false;

    // init timer manager
    if (!TimerManager::Instance()->Init(
                ConfigManager::Instance()->ServerConfig().timer_key(),
                ConfigManager::Instance()->ServerConfig().timer_size())) return false;

    // init table manager
    if (!TableManager::Instance()->Init(
                ConfigManager::Instance()->ServerConfig().table_key(),
                ConfigManager::Instance()->ServerConfig().table_size())) return false;

    // init client connection manager
    if (!ClientConnManager::Instance()->Init(
                ConfigManager::Instance()->ServerConfig().client_conn_key(),
                ConfigManager::Instance()->ServerConfig().client_conn_size())) return false;

    // init words filter
    WordsFilter::Instance()->Clear();
    for (auto& word : ConfigManager::Instance()->GameConfig().dirty_words()) {
        WordsFilter::Instance()->AddWord(word.word());
    }

    // init cs handler
    HandlerManager::AddCSHandler<EnterTableHandler>(msg::CS_TYPE_ENTER_TABLE_REQ, msg::CS_TYPE_ENTER_TABLE_RSP);

    // init ss handler

    return true;
}

void GameServer::OnExit() {
    // serialize container
    SessionManager::Instance()->Release();
    TimerManager::Instance()->Release();
    TableManager::Instance()->Release();
    ClientConnManager::Instance()->Release();
}

int main(int argc, char** argv) { GameServer::Instance()->Run(argc, argv); }

// vim:ts=4:sw=4:et:ft=cpp:
