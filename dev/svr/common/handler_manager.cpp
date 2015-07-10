// File:        handler_manager.cpp
// Description: ---
// Notes:       ---
// Author:      leoxiang <leoxiang727@qq.com>
// Revision:    2015-07-07 by leoxiang

#include "svr/common/handler_manager.h"

////////////////////////////////////////////////////////////
// HandlerManager
HandlerManager::HandlerManager() {
    // init rsp map
    AddSessionRspMap(conf::SERVER_TYPE_ROOM_SERVER, data::SESSION_STATE_ON_ROOMSVR_WAIT, data::SESSION_STATE_ON_ROOMSVR_RSP);

    // init wait map
    AddSessionWaitMap(data::SESSION_STATE_ON_ROOMSVR_WAIT, data::SESSION_STATE_ON_ROOMSVR_TIMEOUT);
}

void HandlerManager::ProcessSession(Session& session) {
    // check input
    if (session.session_type() >= DEF_MAX_HANDLER_NUM) {
        LSF_LOG_FATAL("session type exceed max, %u", session.session_type());
        return;
    }

    // find session handler
    auto iter = _handler_map.find(session.session_type());
    if (iter == _handler_map.end()) {
        LSF_LOG_FATAL("unknown session type, %u", session.session_type());
        return;
    }

    // process session
    iter->second->Process(session);
}

void HandlerManager::ProcessSessionResponse(Session& session) {
    // get server type map
    auto pmap = _session_rsp_map.find(session.ss_response().proxy_head().src_server_type());
    if (pmap == _session_rsp_map.end()) {
        LSF_LOG_ERR("handle session response when cant get server type map, server_type=%u",
                session.ss_response().proxy_head().src_server_type());
        return;
    }

    // check server type and session state
    if (session.session_state() != std::get<0>(pmap->second)) {
        LSF_LOG_ERR("handle session response when state not match, session_state=%u, map_state=%u",
                session.session_state(), std::get<0>(pmap->second));
        return;
    }

    // set state
    session.set_session_state(std::get<1>(pmap->second));

    // process
    ProcessSession(session);
}

void HandlerManager::ProcessSessionTimeout(Session& session) {
    // get session wait map
    auto pmap = _session_wait_map.find(session.session_state());
    if (pmap == _session_wait_map.end()) {
        LSF_LOG_ERR("handle session timeout when cant get session wait map, session_state=%u", session.session_state());
        return;
    }

    // set state
    session.set_session_state(pmap->second);

    // Process
    ProcessSession(session);
}


// vim:ts=4:sw=4:et:ft=cpp:
