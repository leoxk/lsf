// File:        config_center.cpp
// Description: ---
// Notes:       ---
// Author:      leoxiang <leoxiang727@qq.com>
// Revision:    2015-06-09 by leoxiang

#include <fstream>
#include "svr/common/common_header.h"
#include "svr/common/common_func.h"
#include "svr/confsvrd/config_center.h"
#include "svr/confsvrd/name_table_map.h"
#include "svr/confsvrd/csv_config_processor.h"
#include "svr/confsvrd/accept_config_service.h"

using namespace google::protobuf;
using namespace lsf::basic;
using namespace lsf::util;

bool ConfigCenter::InitFromFile() {
    // init deploy config
    if (!Protobuf::ParseProtoMsgFromFile("./conf/deploy.cfg", _deploy_config)) {
        LSF_LOG_ERR("parse failed, %s", LSF_PROTOBUF);
        return false;
    }

    // reload game config
    ReloadGameConfig();

    return true;
}

bool ConfigCenter::GetServerConfig(conf::Server& config, conf::ENServerType server_type, uint32_t server_id) const {
    // copy default
    config.CopyFrom(_deploy_config.default_config());

    // find specified
    for (auto& conf : _deploy_config.server_configs()) {
        if (conf.server_type() == server_type && conf.server_id() == server_id) {
            config.MergeFrom(conf);
            return true;
        }
    }

    return false;
}

#define COPY_CONFIG(name) config.LSF_TOKEN_CAT(mutable_,name)()->CopyFrom(_game_config.name());
#define COPY_CONFIG_WITH_ID(name) \
    for (auto& item : _game_config.name()) {              \
        if (item.server_id() == server_id) {                    \
            config.LSF_TOKEN_CAT(add_,name)()->CopyFrom(item);  \
        }                                                       \
    }

bool ConfigCenter::GetGameConfig(conf::Game& config, conf::ENServerType server_type, uint32_t server_id) const {
    switch (server_type) {
        case conf::SERVER_TYPE_GAME_SERVER:
            COPY_CONFIG(dirty_words);
            COPY_CONFIG_WITH_ID(tables);
            break;

        default:
            break;
    }

    return true;
}

bool ConfigCenter::ReloadGameConfig() {
    // init name table
    NameTableMap::Instance()->Clear();
    if (!NameTableMap::Instance()->Init("./conf/conf_game.proto")) return false;

    // parse game config
    conf::Game game_config;
    if (!CSVConfigProcessor::Instance()->ProcessConfig(game_config, "./table/table.csv", "conf.Table")) return false;
    if (!CSVConfigProcessor::Instance()->ProcessConfig(game_config, "./table/dirty_word.csv", "conf.DirtyWord")) return false;

    // reload success, assign to real config
    _game_config.CopyFrom(game_config);

    // release name table
    NameTableMap::Instance()->Clear();

    AcceptConfigService::Instance()->ReleaseAllConnections();
    return true;
}

 // vim:ts=4:sw=4:et:ft=cpp:
