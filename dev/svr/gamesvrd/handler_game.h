// File:        handler_game.h
// Description: ---
// Notes:       ---
// Author:      leoxiang <leoxiang727@qq.com>
// Revision:    2015-07-20 by leoxiang

#include "svr/common/basic_handler.h"

////////////////////////////////////////////////////////////
class EnterTableHandler : public BasicHandler {
public:
    virtual data::ENSessionState OnClientRequest(Session& session);
};

// vim:ts=4:sw=4:et:ft=cpp:
