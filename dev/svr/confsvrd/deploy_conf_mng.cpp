// File:        deploy_conf_mng.cpp
// Description: ---
// Notes:       ---
// Author:      leoxiang <leoxiang727@qq.com>
// Revision:    2015-06-09 by leoxiang

#include <fstream>
#include "svr/common/common_header.h"
#include "svr/confsvrd/deploy_conf_mng.h"

using namespace google::protobuf;
using namespace lsf::basic;
using namespace lsf::util;

bool DeployConfigManager::Init(char const* path) {
    // open file
    std::ifstream ifs(path);
    if (!ifs) {
        LSF_LOG_ERR("open failed, path=%s, %s", path, strerror(errno));
        return false;
    }

    // parse protobuf
    io::IstreamInputStream isis(&ifs);
    if (!TextFormat::Parse(&isis, &_deploy_config)) {
        LSF_LOG_ERR("parse failed, path=%s, %s", path, ProtobufLog::Instance()->ErrCharStr());
        return false;
    }

    // merge default conf
    for (auto & conf : *_deploy_config.mutable_server_config()) {
        conf.MergeFrom(_deploy_config.default_config());
    }

    return true;
}

conf::Server const* DeployConfigManager::GetServerConfig(conf::ENServerType server_type, uint32_t server_id) const {
    for (auto const & conf : _deploy_config.server_config()) {
        if (conf.server_type() == server_type && conf.server_id() == server_id) return &conf;
    }
    return nullptr;
}

// vim:ts=4:sw=4:et:ft=cpp:
