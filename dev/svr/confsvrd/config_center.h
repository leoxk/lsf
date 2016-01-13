// File:        config_center.h
// Description: ---
// Notes:       ---
// Author:      leoxiang <leoxiang727@qq.com>
// Revision:    2015-06-09 by leoxiang

#include "lsf/algorithm/two_dimensional_table.hpp"
#include "lsf/basic/singleton.hpp"
#include "svr/common/common_proto.h"

////////////////////////////////////////////////////////////
// ConfigCenter
class ConfigCenter : public lsf::basic::Singleton<ConfigCenter> {
public:
    bool InitFromFile();
    bool GetServerConfig(conf::Server& config, conf::ENServerType server_type, uint32_t server_id) const;
    bool GetGameConfig(conf::Game& config, conf::ENServerType server_type, uint32_t server_id) const;

    conf::Deploy const& DeployConfig() const { return _deploy_config; }
    conf::Game const& GameConfig() const { return _game_config; }

    bool ReloadGameConfig();

private:
    conf::Deploy _deploy_config;
    conf::Game   _game_config;
    lsf::algorithm::TwoDimensionalTable<int> _match_pool;
};

// vim:ts=4:sw=4:et:ft=cpp:
