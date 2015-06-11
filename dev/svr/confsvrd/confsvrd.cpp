// File:        confsvrd.cpp
// Description: ---
// Notes:       ---
// Author:      leoxiang <leoxiang727@qq.com>
// Revision:    2015-06-08 by leoxiang

#include <iostream>
#include "svr/common/common_header.h"
#include "svr/confsvrd/confsvrd.h"
#include "svr/confsvrd/deploy_conf_mng.h"

using namespace google::protobuf;
using namespace lsf::basic;
using namespace lsf::util;

bool ConfigServer::OnParseCommond(int argc, char** argv)
{
    // check input
    if (argc < 2)
    {
        std::cerr << "usage: " << argv[0] << " [path]" << std::endl;
        return false;
    }
    
    // parse content
    _server_id = 0;
    _server_name = StringExt::GetBaseName(argv[0]);

    // convert to abs path
    _config_path = System::GetAbsPath(argv[1]);

    return true;
}

bool ConfigServer::OnGetConfig()
{
    // load game config
    if (!DeployConfigManager::Instance()->Init(_config_path.c_str())) return false;

    // get server config from deploy config
    conf::Server const * pconf = DeployConfigManager::Instance()->GetServerConfig(_server_type, _server_id);
    if (!pconf)
    {
        LSF_LOG_ERR("server_type=%u, server_id=%u, %s", _server_type, _server_id);
        return false;
    }
    _server_config.CopyFrom(*pconf);

    // check server type and id
    if (_server_type != _server_config.server_type() ||
        _server_id != _server_config.server_id())
    {
        LSF_LOG_ERR("input=%u %u, config=%u %u", _server_type, _server_id, 
                _server_config.server_type(), _server_config.server_id());
        return false;
    }

    LSF_LOG_INFO("get config from %s", _config_path.c_str());
    return true;
}

bool ConfigServer::OnRun()
{
    // output all config
    return true;
}

bool ConfigServer::OnClientMessage()
{
    return true;
}

int main(int argc, char** argv)
{
    ConfigServer::Instance()->Run(argc, argv);
}

// vim:ts=4:sw=4:et:ft=cpp:
