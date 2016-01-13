// File:        data_server.cpp
// Description: ---
// Notes:       ---
// Author:      leoxiang <leoxiang727@qq.com>
// Revision:    2015-08-06 by leoxiang

#include "svr/common/common_header.h"
#include "svr/common/common_service.h"
#include "svr/common/session_manager.h"
#include "svr/common/config_manager.h"
#include "svr/common/timer_manager.h"
#include "svr/common/handler_manager.h"
#include "svr/datasvrd/data_server.h"
#include "svr/datasvrd/handler_common.h"
#include "svr/datasvrd/leveldb_manager.h"

using namespace google::protobuf;
using namespace lsf::util;
using namespace lsf::asio;

bool DataServer::OnRun() {
    // init service
    if (!ConnectServerMsgTransferService::Instance()->Run(this)) return false;

    // init session manager
    if (!SessionManager::Instance()->Init(
                ConfigManager::Instance()->ServerConfig().session_key(),
                ConfigManager::Instance()->ServerConfig().session_size())) return false;

    // init timer manager
    if (!TimerManager::Instance()->Init(
                ConfigManager::Instance()->ServerConfig().timer_key(),
                ConfigManager::Instance()->ServerConfig().timer_size())) return false;

    // init leveldb manager
    if (!LevelDBManager::Instance()->Init(
                ConfigManager::Instance()->ServerConfig().db_cache(),
                ConfigManager::Instance()->ServerConfig().db_path())) return false;

    // init ss handler
    HandlerManager::AddSSHandler<QueryDataHandler>(msg::SS_TYPE_QUERY_DATA_REQ, msg::SS_TYPE_QUERY_DATA_RSP);
    HandlerManager::AddSSHandler<UpdateDataHandler>(msg::SS_TYPE_UPDATE_DATA_REQ, msg::SS_TYPE_UPDATE_DATA_RSP);
    HandlerManager::AddSSHandler<DeleteDataHandler>(msg::SS_TYPE_DELETE_DATA_REQ, msg::SS_TYPE_DELETE_DATA_RSP);
    HandlerManager::AddSSHandler<InsertDataHandler>(msg::SS_TYPE_INSERT_DATA_REQ, msg::SS_TYPE_INSERT_DATA_RSP);
    HandlerManager::AddSSHandler<UnlockDataHandler>(msg::SS_TYPE_UNLOCK_DATA_NOTIFY);

    return true;
}

int main(int argc, char** argv) { DataServer::Instance()->Run(argc, argv); }

// vim:ts=4:sw=4:et:ft=cpp:
