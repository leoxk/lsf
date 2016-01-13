// File:        http_server.cpp
// Description: ---
// Notes:       ---
// Author:      leoxiang <leoxiang727@qq.com>
// Revision:    2015-09-04 by leoxiang

#include "svr/common/common_service.h"
#include "svr/common/session_manager.h"
#include "svr/common/timer_manager.h"
#include "svr/common/handler_manager.h"
#include "svr/common/config_manager.h"
#include "svr/httpsvrd/http_server.h"
#include "svr/httpsvrd/http_service.h"

using namespace google::protobuf;
using namespace lsf::util;
using namespace lsf::asio;

bool HttpServer::OnRun() {
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

    // init curl
    if (!CurlMulti::Instance()->Init()) {
        LSF_LOG_ERR("init CurlMulti failed, %s", LSF_ES(CurlMulti::Reference()));
        return false;
    }

    if(!HttpService::Instance()->Init()){
        LSF_LOG_ERR("Init HttpService failed!");
        return false;
    }

    // init ss handler

    return true;
}

void HttpServer::OnExit(){
    HttpService::Instance()->Exit();
}

int main(int argc, char** argv) { HttpServer::Instance()->Run(argc, argv); }

// vim:ts=4:sw=4:et:ft=cpp:
