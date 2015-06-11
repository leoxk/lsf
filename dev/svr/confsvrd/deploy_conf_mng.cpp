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

bool DeployConfigManager::Init(char const * path)
{
    // open file
    std::ifstream ifs(path);
    if (!ifs) {
        LSF_LOG_ERR("path=%s, %s", path, strerror(errno));
        return false;
    }

    // parse protobuf
    io::IstreamInputStream isis(&ifs);
    if (!TextFormat::Parse(&isis, &_deploy_config))
    {
        LSF_LOG_ERR("path=%s, %s", path, ProtobufLog::Instance()->ErrCharStr());
        return false;
    }

    // merge default conf
    conf::Server const & def_conf = _deploy_config.default_config();
    for (int i = 0; i < _deploy_config.server_config_size(); ++i)
    {
        _deploy_config.mutable_server_config(i)->MergeFrom(def_conf);
    }

    return true;
}

conf::Server const * DeployConfigManager::GetServerConfig(conf::ENServerType server_type, uint32_t server_id)
{
    for (int i = 0; i < _deploy_config.server_config_size(); ++i)
    {
        conf::Server const & conf = _deploy_config.server_config(i);
        if (conf.server_type() == server_type && 
            conf.server_id() == server_id)
        {
            return &conf;
        }
    }
    return NULL;
}

// vim:ts=4:sw=4:et:ft=cpp:
