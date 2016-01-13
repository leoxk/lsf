// File:        config_server.cpp
// Description: ---
// Notes:       ---
// Author:      leoxiang <leoxiang727@qq.com>
// Revision:    2015-06-08 by leoxiang

#include <iostream>
#include "svr/common/common_header.h"
#include "svr/common/common_service.h"
#include "svr/common/config_manager.h"
#include "svr/common/handler_manager.h"
#include "svr/confsvrd/config_server.h"
#include "svr/confsvrd/config_center.h"
#include "svr/confsvrd/accept_config_service.h"

using namespace google::protobuf;
using namespace lsf::basic;
using namespace lsf::util;

bool ConfigServer::OnParseCommond() {
    // parse content
    _server_id = 0;
    _server_name = StringExt::GetBaseName(_argv[0]);

    return true;
}

bool ConfigServer::OnInitConfigService() {
    // load deploy config and game config from files
    if (!ConfigCenter::Instance()->InitFromFile()) return false;

    // get server config from deploy
    conf::Server conf;
    if (!ConfigCenter::Instance()->GetServerConfig(conf, _server_type, _server_id)) {
        LSF_LOG_ERR("get config failed, server_type=%s, server_id=%u, %s", LSF_ETS(_server_type), _server_id);
        return false;
    }
    ConfigManager::Instance()->SetServerConfig(conf);

    // check server type and id
    auto const& server_config = ConfigManager::Instance()->ServerConfig();
    if (_server_type != server_config.server_type() || _server_id != server_config.server_id()) {
        LSF_LOG_ERR("server not match, input=%s %u, config=%s %u", LSF_ETS(_server_type), _server_id,
                    LSF_ETS(server_config.server_type()), server_config.server_id());
        return false;
    }

    // init session manager
    if (!SessionManager::Instance()->Init(
                ConfigManager::Instance()->ServerConfig().session_key(),
                ConfigManager::Instance()->ServerConfig().session_size())) return false;

    return true;
}

bool ConfigServer::OnRun() {
    // init service
    if (!AcceptConfigService::Instance()->Run(this)) return false;
    ConnectServerMsgTransferService::Instance()->Run(this);

    return true;
}

int main(int argc, char** argv) { ConfigServer::Instance()->Run(argc, argv); }

// vim:ts=4:sw=4:et:ft=cpp:
