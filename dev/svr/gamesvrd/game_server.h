// File:        game_server.h
// Description: ---
// Notes:       ---
// Author:      leoxiang <leoxiang727@qq.com>
// Revision:    2015-06-25 by leoxiang

#include "lsf/basic/singleton.hpp"
#include "svr/common/basic_server.h"

////////////////////////////////////////////////////////////
// GameServer
class GameServer : public BasicServer, public lsf::basic::Singleton<GameServer> {
public:
    GameServer() : BasicServer(conf::SERVER_TYPE_GAME_SERVER) {}

public:
    virtual bool OnRun();
    virtual void OnExit();
};

// vim:ts=4:sw=4:et:ft=cpp:
