// File:        basic_handler_db.cpp
// Description: ---
// Notes:       ---
// Author:      leoxiang <leoxiang727@qq.com>
// Revision:    2015-07-09 by leoxiang

#include "svr/common/basic_handler_db.h"

data::ENSessionState BasicHandlerDB::OnClientMessage(Session& session) {
    // set state
    session.set_db_query_finish(false);
    data::ENSessionState state = OnPreProcess(session);
    return state;
}

data::ENSessionState BasicHandlerDB::OnServerMessage(Session& session) {
    return OnClientMessage(session);
}

// vim:ts=4:sw=4:et:ft=cpp:
