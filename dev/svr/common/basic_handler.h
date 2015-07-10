// File:        basic_handler.h
// Description: ---
// Notes:       ---
// Author:      leoxiang <leoxiang727@qq.com>
// Revision:    2015-07-07 by leoxiang

#pragma once
#include "svr/common/common_header.h"
#include "svr/common/session_manager.h"

////////////////////////////////////////////////////////////
// BasicHandler
class BasicHandler {
public:
    virtual ~BasicHandler() { }
    void Process(Session& session);

public:
    // get request message
    virtual data::ENSessionState OnClientMessage(Session& session) { return data::SESSION_STATE_FINISH; }
    virtual data::ENSessionState OnServerMessage(Session& session) { return data::SESSION_STATE_FINISH; }

    // roomsvr response
    virtual data::ENSessionState OnRoomSvrMessage(Session& session) { return data::SESSION_STATE_FINISH; }
    virtual data::ENSessionState OnRoomSvrTimeout(Session& session) { return data::SESSION_STATE_FINISH; }

    // dbsvr response
    virtual data::ENSessionState OnDBSvrMessage(Session& session) { return data::SESSION_STATE_FINISH; }
    virtual data::ENSessionState OnDBSvrTimeout(Session& session) { return data::SESSION_STATE_FINISH; }

    // virtual data::ENSessionState OnDBSvrQueryMessage(Session& session);
    // virtual data::ENSessionState OnDBSvrUpdateMessage(Session& session);
    // virtual data::ENSessionState OnDBSvrDeleteMessage(Session& session);
    //
    // virtual data::ENSessionState OnDBSvrQueryTimeout(Session& session);
    // virtual data::ENSessionState OnDBSvrUpdateTimeout(Session& session);
    // virtual data::ENSessionState OnDBSvrDeleteTimeout(Session& session);
};

// vim:ts=4:sw=4:et:ft=cpp:
