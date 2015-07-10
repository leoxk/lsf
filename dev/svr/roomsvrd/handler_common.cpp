// File:        handler_common.cpp
// Description: ---
// Notes:       ---
// Author:      leoxiang <leoxiang727@qq.com>
// Revision:    2015-07-10 by leoxiang

#include "svr/roomsvrd/handler_common.h"
#include "svr/common/common_service.h"

data::ENSessionState QuickStartHandler::OnClientMessage(Session& session) {
    // msg::QuickStartReq const& request = session.cs_request().quick_start_req();
    msg::QuickStartRsp & response = *session.mutable_cs_response()->mutable_quick_start_rsp();

    response.set_table_id(0);
    response.set_seat_index(0);

    ConnectClientMsgTransferService::Instance()->SendMessage(session);
    return data::SESSION_STATE_FINISH;
}

// vim:ts=4:sw=4:et:ft=cpp:
