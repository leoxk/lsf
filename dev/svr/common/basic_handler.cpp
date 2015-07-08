// File:        basic_handler.cpp
// Description: ---
// Notes:       ---
// Author:      leoxiang <leoxiang727@qq.com>
// Revision:    2015-07-07 by leoxiang

#include "svr/common/basic_handler.h"

////////////////////////////////////////////////////////////
// BasicHandler
void BasicHandler::Process(Session& session) {
    // state machine
    while (true) {
        // repeate until wait
        data::ENSessionState state = data::SESSION_STATE_NONE;
        switch (session.session_state()) {
            case data::SESSION_STATE_ON_CLIENT_REQ:           state = OnClientMessage(session); break;
            case data::SESSION_STATE_ON_SERVER_REQ:           state = OnServerMessage(session); break;
            case data::SESSION_STATE_ON_ROOMSVR_TIMEOUT:      state = OnRoomSvrTimeout(session); break;
            case data::SESSION_STATE_ON_ROOMSVR_RSP:          state = OnRoomSvrMessage(session); break;
            case data::SESSION_STATE_ON_DBSVR_QUERY_TIMEOUT:  state = OnDBSvrQueryTimeout(session); break;
            case data::SESSION_STATE_ON_DBSVR_QUERY_RSP:      state = OnDBSvrQueryMessage(session); break;
            case data::SESSION_STATE_ON_DBSVR_UPDATE_TIMEOUT: state = OnDBSvrUpdateTimeout(session); break;
            case data::SESSION_STATE_ON_DBSVR_UPDATE_RSP:     state = OnDBSvrUpdateMessage(session); break;
            case data::SESSION_STATE_ON_DBSVR_DELETE_TIMEOUT: state = OnDBSvrDeleteTimeout(session); break;
            case data::SESSION_STATE_ON_DBSVR_DELETE_RSP:     state = OnDBSvrDeleteMessage(session); break;

            case data::SESSION_STATE_ON_ROOMSVR_WAIT:
            case data::SESSION_STATE_ON_DBSVR_QUERY_WAIT:
            case data::SESSION_STATE_ON_DBSVR_UPDATE_WAIT:
            case data::SESSION_STATE_ON_DBSVR_DELETE_WAIT:
            case data::SESSION_STATE_FINISH:
            case data::SESSION_STATE_NONE:
            default: return;
        }

        if (state != data::SESSION_STATE_NONE) session.set_session_state(state);
    }
}

void BasicHandler::ProcessResponse(Session& session) {
    // TODO
    switch (session.ss_response().proxy_head().src_server_type()) {
        case conf::SERVER_TYPE_ROOM_SERVER: break;
        default: break;
    }

}

void BasicHandler::ProcessTimeout(Session& session) {

}

////////////////////////////////////////////////////////////
// HandlerManager
BasicHandler* HandlerManager::_GetHandler(Session const& session) {
    // check input
    if (session.session_type() >= DEF_MAX_HANDLER_NUM) {
        LSF_LOG_FATAL("session type exceed max, %u", session.session_type());
        return nullptr;
    }

    // find session handler
    BasicHandler* phandler = _handler_array[session.session_type()];
    if (phandler == nullptr) {
        LSF_LOG_FATAL("unknown session type, %u", session.session_type());
        return nullptr;
    }

    return phandler;
}

void HandlerManager::ProcessSession(Session& session) {
    BasicHandler* phandler = _GetHandler(session);
    if (phandler) phandler->Process(session);
}

void HandlerManager::ProcessSessionResponse(Session& session) {
    BasicHandler* phandler = _GetHandler(session);
    if (phandler) phandler->ProcessResponse(session);
}

void HandlerManager::ProcessSessionTimeout(Session& session) {
    BasicHandler* phandler = _GetHandler(session);
    if (phandler) phandler->ProcessTimeout(session);
}

// vim:ts=4:sw=4:et:ft=cpp:
