// File:        connect_client_service.cpp
// Description: ---
// Notes:       ---
// Author:      leoxiang <leoxiang727@qq.com>
// Revision:    2016-03-11 by leoxiang

#include "svr/common/common_header.h"
#include "svr/common/common_func.h"
#include "svr/common/config_manager.h"
#include "svr/testsvrd/connect_client_service.h"

using namespace lsf::asio;
using namespace lsf::basic;
using namespace lsf::util;

bool ConnectClientService::OnRun() {
    // register sending heart beat
    IOService::Instance()->AsyncAddTimerForever(1*1000, [this](int) {
            if (_conn_scok.empty()) return;

            auto socket = _conn_scok[0];
            if (!socket->IsConnect()) return;

            msg::CS message;
            message.set_msg_type(msg::CS_TYPE_HEART_BEAT_REQ);
            ConnectionSend(socket, message);
        });

    return true;
}

void ConnectClientService::OnConnectionCreate(lsf::asio::SharedSocket socket) {
}

void ConnectClientService::OnConnectionMessage(lsf::asio::SharedSocket socket, std::string& message) {
    // unpack msg
    msg::CS cs_msg;
    if (!Protobuf::ParseProtoMsgFromString(message, cs_msg)) {
        LSF_LOG_ERR("parse failed, %s", LSF_PROTOBUF);
        return;
    }

    // LSF_LOG_INF("get message uid=%lu, msg_type=%s(%u)", base_type::uid(), LSF_ETS(cs_msg.msg_type()), (int)cs_msg.msg_type());

    // process message
    switch (cs_msg.msg_type()) {
    //     case msg::CS_TYPE_LOGIN_REQ:        HandleLogin(socket, cs_msg); break;
    //     case msg::CS_TYPE_HEART_BEAT_REQ:   HandleHeartBeat(socket, cs_msg); break;
    //     case msg::CS_TYPE_TIME_SYNC_REQ:    HandleTimeSync(socket, cs_msg); break;
        default:                            HandleOther(socket, cs_msg); break;
    }
}

void ConnectClientService::HandleOther(lsf::asio::SharedSocket socket, msg::CS& message) {
}

void ConnectClientService::OnConnectionPeerClose(lsf::asio::SharedSocket socket) {
}

// vim:ts=4:sw=4:et:ft=cpp:
