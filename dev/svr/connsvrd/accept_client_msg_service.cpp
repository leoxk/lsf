// File:        accept_client_msg_service.cpp
// Description: ---
// Notes:       ---
// Author:      leoxiang <leoxiang727@qq.com>
// Revision:    2015-06-23 by leoxiang

#include <sstream>
#include <openssl/md5.h>
#include "svr/common/common_header.h"
#include "svr/common/common_func.h"
#include "svr/common/config_manager.h"
#include "svr/connsvrd/accept_client_msg_service.h"
#include "svr/connsvrd/accept_client_msg_transfer_service.h"

using namespace lsf::asio;
using namespace lsf::basic;
using namespace lsf::util;

bool AcceptClientMsgService::OnRun() {
    // add heart beat check if needed
    if (ConfigManager::Instance()->ServerConfig().need_heart_beat()) {
        IOService::Instance()->AsyncAddTimerForever(DEF_HEART_BEAT_INTERVAL, [this](int) {
                // check all connection
                std::vector<SharedSocket> close_socks;
                for (auto& pair : _sock_map) {
                    uint64_t diff = IOService::Instance()->ClockTimeMilli() - pair.second.last_heart_beat_time;
                    if (diff > DEF_HEART_BEAT_TIMEOUT) {
                        close_socks.push_back(pair.second.socket);
                        LSF_LOG_INF("player heart beat timeout, uid=%u, conn_id=%u",
                                pair.second.conn_head.uid(),
                                pair.second.conn_head.conn_id());
                    }
                }

                // close timeout conn
                for (auto& sock : close_socks) {
                    OnConnectionPeerClose(sock);
                    ConnectionClose(sock);
                }
            });
    }
    return true;
}

void AcceptClientMsgService::OnConnectionCreate(lsf::asio::SharedSocket socket) {
    // add map entery
    auto& info = _sock_map[socket->SockFd()];
    info.socket = socket;
    info.conn_head.set_conn_id(socket->SockFd());
    info.conn_head.set_conn_ip(socket->RemoteSockAddr().GetAddress().ToString());
    info.conn_head.set_conn_port(socket->RemoteSockAddr().GetPort());
    info.conn_head.set_is_new_conn(true); // set new conn flag
    info.last_heart_beat_time = IOService::Instance()->ClockTimeMilli();
}

void AcceptClientMsgService::OnConnectionMessage(lsf::asio::SharedSocket socket, std::string& message) {
    // unpack msg
    msg::CS cs_msg;
    if (!Protobuf::ParseProtoMsgFromString(message, cs_msg)) {
        LSF_LOG_ERR("parse failed, %s", LSF_PROTOBUF);
        return;
    }

    LSF_LOG_INF("get message from conn_id=%u, msg_type=%s(%u)", socket->SockFd(), LSF_ETS(cs_msg.msg_type()), (int)cs_msg.msg_type());

    // process login
    switch (cs_msg.msg_type()) {
        case msg::CS_TYPE_LOGIN_REQ:        HandleLogin(socket, cs_msg); break;
        case msg::CS_TYPE_HEART_BEAT_REQ:   HandleHeartBeat(socket, cs_msg); break;
        case msg::CS_TYPE_TIME_SYNC_REQ:    HandleTimeSync(socket, cs_msg); break;
        default:                            HandleOther(socket, cs_msg); break;
    }
}

void AcceptClientMsgService::HandleHeartBeat(lsf::asio::SharedSocket socket, msg::CS& message) {
    // send response
    msg::CS cs_response;
    cs_response.set_msg_type(msg::CS_TYPE_HEART_BEAT_RSP);
    cs_response.mutable_heart_beat_rsp();
    ConnectionSend(socket, cs_response);

    // update last heart beat time
    _sock_map[socket->SockFd()].last_heart_beat_time = IOService::Instance()->ClockTimeMilli();
}

void AcceptClientMsgService::HandleTimeSync(lsf::asio::SharedSocket socket, msg::CS& message) {
    // send response
    msg::CS cs_response;
    cs_response.set_msg_type(msg::CS_TYPE_TIME_SYNC_RSP);
    cs_response.mutable_time_sync_rsp()->set_server_time(IOService::Instance()->ClockTime());
    cs_response.mutable_time_sync_rsp()->set_server_time_milli(IOService::Instance()->ClockTimeMilli());
    ConnectionSend(socket, cs_response);
}

void AcceptClientMsgService::HandleLogin(lsf::asio::SharedSocket socket, msg::CS& message) {
    msg::CS cs_response;
    cs_response.set_msg_type(msg::CS_TYPE_LOGIN_RSP);
    auto& response = *cs_response.mutable_login_rsp();
    auto& request = message.login_req();

    // set response
    msg::ConnHead& conn_head = _sock_map[socket->SockFd()].conn_head;
    cs_response.mutable_conn_head()->CopyFrom(conn_head);
    LSF_SCOPE_EXIT_NAME(exit) {
        ConnectionSend(socket, cs_response);
        ConnectionClose(socket);
    };

    // check input
    if (!request.has_uid())   { response.set_result(msg::ERROR_INVALID_UID); return; }

    // verify sig if needed
    if (ConfigManager::Instance()->ServerConfig().need_login_verify() &&
        !ConfigManager::Instance()->IsTest(request.uid())) {
        // check input
        // if (!request.has_uname()) { response.set_result(msg::ERROR_INVALID_UNAME); return; }
        // if (!request.has_verify_sig()) { response.set_result(msg::ERROR_INVALID_VERIFY_SIG); return; }

        // construct pre sig
        std::ostringstream oss;
        oss << "uid=" << request.uid()
            << "|username=" << request.uname()
            << "|app_secret_key=" << ConfigManager::Instance()->ServerConfig().app_secret_key();
        std::string pre_sig = oss.str();

        // calc sig
        uint8_t buffer[MD5_DIGEST_LENGTH];
        MD5((uint8_t const*)pre_sig.data(), pre_sig.size(), buffer);
        std::string sig = StringExt::BinToHexString(buffer, sizeof(buffer));
        LSF_LOG_INF("verify signature: %s %s %s", pre_sig.c_str(), sig.c_str(), request.verify_sig().c_str());

        // verify sig
        // if (sig != request.verify_sig()) { response.set_result(msg::ERROR_INVALID_VERIFY_SIG); return; }
    }

    // assign value
    conn_head.set_uid(request.uid());
    conn_head.set_uname(request.uname());
    conn_head.set_is_login(true);
    if (request.has_platform()) conn_head.set_platform(request.platform());
    if (request.has_client_version()) conn_head.set_client_version(request.client_version());

    // send response
    exit.Dismiss();
    ConnectionSend(socket, cs_response);
}


void AcceptClientMsgService::HandleOther(lsf::asio::SharedSocket socket, msg::CS& message) {
    // check login
    msg::ConnHead& conn_head = _sock_map[socket->SockFd()].conn_head;
    if (!conn_head.is_login()) {
        LSF_LOG_ERR("message send without login, uid=%u, msg_type=%s", conn_head.uid(), LSF_ETS(message.msg_type()));
        return;
    }

    // copy conn head
    message.mutable_conn_head()->CopyFrom(conn_head);
    if (conn_head.is_new_conn()) conn_head.clear_is_new_conn();

    // transfer
    AcceptClientMsgTransferService::Instance()->TransferMessage(message);
}

void AcceptClientMsgService::OnConnectionClose(lsf::asio::SharedSocket socket) {
    // erase map entry
    _sock_map.erase(socket->SockFd());
}

void AcceptClientMsgService::OnConnectionPeerClose(lsf::asio::SharedSocket socket) {
    // send close connnection notify
    msg::ConnHead const& conn_head = _sock_map[socket->SockFd()].conn_head;
    if (!conn_head.is_new_conn()) {
        msg::CS message;
        message.mutable_conn_head()->CopyFrom(conn_head);
        message.mutable_conn_head()->set_is_close_conn(true);
        AcceptClientMsgTransferService::Instance()->TransferMessage(message);
    }
}

bool AcceptClientMsgService::SendMessageToClient(msg::CS& message) {
    // get socket from head
    if (_sock_map.find(message.conn_head().conn_id()) == _sock_map.end()) {
        LSF_LOG_ERR("socket get from conn head err, fd=%d", message.conn_head().conn_id());
        return true;
    }
    SharedSocket socket = _sock_map[(int)message.conn_head().conn_id()].socket;

    LSF_LOG_INF("send message to conn_id=%u, msg_type=%s(%u)", message.conn_head().conn_id(), LSF_ETS(message.msg_type()), (int)message.msg_type());

    // send message
    message.clear_conn_head();
    ConnectionSend(socket, message);
    return true;
}

// vim:ts=4:sw=4:et:ft=cpp:
