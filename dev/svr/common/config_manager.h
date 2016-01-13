// File:        config_manager.h
// Description: ---
// Notes:       ---
// Author:      leoxiang <leoxiang727@qq.com>
// Revision:    2015-07-13 by leoxiang

#include "lsf/algorithm/two_dimensional_table.hpp"
#include "svr/common/common_proto.h"
#include "svr/common/common_header.h"

////////////////////////////////////////////////////////////
// ConfigManager
class ConfigManager : public lsf::basic::Singleton<ConfigManager> {
public:
    void SetGameConfig(conf::Game const& game_config) { GameConfigMerge(game_config); }
    void SetServerConfig(conf::Server const& server_config) { _server_config.CopyFrom(server_config); }
    conf::Game   const& GameConfig() const { return _game_config; }
    conf::Server const& ServerConfig() const { return _server_config; }

    // test uid
    bool IsTest(uint64_t uid);

private:
    void GameConfigMerge(conf::Game const& game_config);

private:
    conf::Game   _game_config;
    conf::Server _server_config;
};

// vim:ts=4:sw=4:et:ft=cpp:
