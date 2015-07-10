// File:        basic_handler.cpp
// Description: ---
// Notes:       ---
// Author:      leoxiang <leoxiang727@qq.com>
// Revision:    2015-07-07 by leoxiang

#include "svr/common/basic_handler.h"
#include "svr/common/common_service.h"

////////////////////////////////////////////////////////////
// BasicHandler
void BasicHandler::Process(Session& session) {
    // state machine
    while (true) {
        // repeate until wait
        data::ENSessionState state = data::SESSION_STATE_NONE;
        switch (session.session_state()) {
            case data::SESSION_STATE_ON_CLIENT_REQ:      state = OnClientMessage(session);  break;
            case data::SESSION_STATE_ON_SERVER_REQ:      state = OnServerMessage(session);  break;
            case data::SESSION_STATE_ON_ROOMSVR_TIMEOUT: state = OnRoomSvrTimeout(session); break;
            case data::SESSION_STATE_ON_ROOMSVR_RSP:     state = OnRoomSvrMessage(session); break;
            case data::SESSION_STATE_ON_DBSVR_TIMEOUT:   state = OnDBSvrTimeout(session);   break;
            case data::SESSION_STATE_ON_DBSVR_RSP:       state = OnDBSvrMessage(session);   break;

            // case data::SESSION_STATE_ON_DBSVR_QUERY_TIMEOUT:  state = OnDBSvrQueryTimeout(session); break;
            // case data::SESSION_STATE_ON_DBSVR_QUERY_RSP:      state = OnDBSvrQueryMessage(session); break;
            // case data::SESSION_STATE_ON_DBSVR_UPDATE_TIMEOUT: state = OnDBSvrUpdateTimeout(session); break;
            // case data::SESSION_STATE_ON_DBSVR_UPDATE_RSP:     state = OnDBSvrUpdateMessage(session); break;
            // case data::SESSION_STATE_ON_DBSVR_DELETE_TIMEOUT: state = OnDBSvrDeleteTimeout(session); break;
            // case data::SESSION_STATE_ON_DBSVR_DELETE_RSP:     state = OnDBSvrDeleteMessage(session); break;
            // case data::SESSION_STATE_ON_DBSVR_QUERY_WAIT:
            // case data::SESSION_STATE_ON_DBSVR_UPDATE_WAIT:
            // case data::SESSION_STATE_ON_DBSVR_DELETE_WAIT:

            case data::SESSION_STATE_ON_ROOMSVR_WAIT:
            case data::SESSION_STATE_ON_DBSVR_QUERY_WAIT:
            case data::SESSION_STATE_FINISH:
            case data::SESSION_STATE_NONE: break;
            default:
                LSF_LOG_ERR("unknow session state, %u", session.session_state());
                state = data::SESSION_STATE_FINISH;
                break;
        }

        // set state
        if (state != data::SESSION_STATE_NONE) session.set_session_state(state);

        // check finish
        if (session.session_state() == data::SESSION_STATE_FINISH) {
            SessionManager::Instance()->ReleaseSession(session.session_id());
            return;
        }
    }
}

// vim:ts=4:sw=4:et:ft=cpp:
