// File:        handler_game.cpp
// Description: ---
// Notes:       ---
// Author:      leoxiang <leoxiang727@qq.com>
// Revision:    2015-07-20 by leoxiang

#include "svr/common/common_header.h"
#include "svr/common/common_proto.h"
#include "svr/common/session_manager.h"
#include "svr/common/common_func.h"
#include "svr/common/config_manager.h"
#include "svr/gamesvrd/handler_game.h"
#include "svr/gamesvrd/table_manager.h"

////////////////////////////////////////////////////////////
data::ENSessionState EnterTableHandler::OnClientRequest(Session& session) {
    // auto& request = session.cs_request().enter_table_req();
    // auto& response = *session.mutable_cs_response()->mutable_enter_table_rsp();
    // auto conn_id = session.cs_request().conn_head().conn_id();
    // auto uid = session.cs_request().conn_head().uid();

    return data::SESSION_STATE_FINISH;
}

// vim:ts=4:sw=4:et:ft=cpp:
