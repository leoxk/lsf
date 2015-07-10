// File:        basic_handler_db.h
// Description: ---
// Notes:       ---
// Author:      leoxiang <leoxiang727@qq.com>
// Revision:    2015-07-09 by leoxiang

#include "svr/common/basic_handler.h"

class BasicHandlerDB : public BasicHandler {
public:
    virtual data::ENSessionState OnPreProcess(Session& session);

protected:
    virtual data::ENSessionState OnClientMessage(Session& session) final;
    virtual data::ENSessionState OnServerMessage(Session& session) final;

};

// vim:ts=4:sw=4:et:ft=cpp:
