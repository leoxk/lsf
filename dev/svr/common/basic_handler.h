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
class BasicHandler
{
public:
    virtual ~BasicHandler() { }
    void Process(Session& session);
    void ProcessResponse(Session& session);
    void ProcessTimeout(Session& session);

public:
    // get request message
    virtual data::ENSessionState OnClientMessage(Session& session);
    virtual data::ENSessionState OnServerMessage(Session& session);

    // roomsvr response
    virtual data::ENSessionState OnRoomSvrMessage(Session& session);
    virtual data::ENSessionState OnRoomSvrTimeout(Session& session);

    // dbsvr response
    virtual data::ENSessionState OnDBSvrQueryMessage(Session& session);
    virtual data::ENSessionState OnDBSvrUpdateMessage(Session& session);
    virtual data::ENSessionState OnDBSvrDeleteMessage(Session& session);

    virtual data::ENSessionState OnDBSvrQueryTimeout(Session& session);
    virtual data::ENSessionState OnDBSvrUpdateTimeout(Session& session);
    virtual data::ENSessionState OnDBSvrDeleteTimeout(Session& session);
};

////////////////////////////////////////////////////////////
// HandlerManager
class HandlerManager : public lsf::basic::Singleton<HandlerManager> {
public:
    static const size_t DEF_MAX_HANDLER_NUM = 5000;

public:
    HandlerManager() { std::fill_n(_handler_array, DEF_MAX_HANDLER_NUM, nullptr); }
    void AddHandler(uint32_t session_type, BasicHandler* phandler) { _handler_array[session_type] = phandler; }
    bool IsRequest(uint32_t msg_id) { return _handler_array[msg_id] != nullptr; }

    void ProcessSession(Session& session);
    void ProcessSessionResponse(Session& session);
    void ProcessSessionTimeout(Session& session);

private:
    BasicHandler* _GetHandler(Session const& session);

private:
    BasicHandler* _handler_array[DEF_MAX_HANDLER_NUM];
};


// vim:ts=4:sw=4:et:ft=cpp:
