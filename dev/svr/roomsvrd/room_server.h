// File:        room_server.h
// Description: ---
// Notes:       ---
// Author:      leoxiang <leoxiang727@qq.com>
// Revision:    2015-07-07 by leoxiang

#include "lsf/basic/singleton.hpp"
#include "svr/common/basic_server.h"

////////////////////////////////////////////////////////////
// RoomServer
class RoomServer : public BasicServer, public lsf::basic::Singleton<RoomServer> {
public:
    RoomServer() : BasicServer(conf::SERVER_TYPE_GAME_SERVER) {}

public:
    virtual bool OnRun();
};


// vim:ts=4:sw=4:et:ft=cpp:
