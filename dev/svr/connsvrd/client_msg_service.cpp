// File:        client_msg_service.cpp
// Description: ---
// Notes:       ---
// Author:      leoxiang <leoxiang727@qq.com>
// Revision:    2015-06-23 by leoxiang

#include "svr/connsvrd/client_msg_service.h"
#include "svr/common/common_header.h"
#include "svr/common/common_func.h"
#include "svr/connsvrd/client_msg_transfer_service.h"

using namespace lsf::asio;

bool AcceptClientMsgService::OnConnectionCreate(lsf::asio::Socket socket) {
    // add map entery
    msg::ConnHead & conn_head = _sock_map[socket];
    conn_head.set_conn_id(socket.GetSockFd());
    conn_head.set_conn_ip(socket.RemoteSockAddr().GetAddress().ToString());
    conn_head.set_conn_port(socket.RemoteSockAddr().GetPort());

    // send new connection notify
    msg::CS message;
    message.mutable_conn_head()->CopyFrom(conn_head);
    message.mutable_conn_head()->set_is_new_conn(true);
    AcceptClientMsgTransferService::Instance()->TransferMessage(message);

    return true;
}

bool AcceptClientMsgService::OnConnectionMessage(lsf::asio::Socket socket, std::string& message) {
    // unpack msg
    msg::CS cs_msg;
    if (!common::UnPackProtoMsg(message, cs_msg)) return true;

    // copy conn head
    cs_msg.mutable_conn_head()->CopyFrom(_sock_map[socket]);
    // TODO if verify sig only used in connsvrd, here should clear it

    // transfer
    AcceptClientMsgTransferService::Instance()->TransferMessage(cs_msg);

    return true;
}

void AcceptClientMsgService::OnConnectionClose(lsf::asio::Socket socket) {
    // send close connnection notify
    msg::CS message;
    message.mutable_conn_head()->CopyFrom(_sock_map[socket]);
    message.mutable_conn_head()->set_is_close_conn(true);
    AcceptClientMsgTransferService::Instance()->TransferMessage(message);

    // erase map entry
    _sock_map.erase(socket);
}

bool AcceptClientMsgService::SendResposeToClient(std::string &message) {
    // unpack
    msg::CS cs_msg;
    if (!common::UnPackProtoMsg(message, cs_msg)) return true;

    // get socket from head
    Socket socket = { (int)cs_msg.conn_head().conn_id() };
    if (!socket) {
        LSF_LOG_ERR("socket get from conn head err, fd=%u", socket.GetSockFd());
        return true;
    }
    bool close_connection = cs_msg.conn_head().is_close_conn();

    // pack new message
    std::string buffer;
    cs_msg.clear_conn_head();
    if (!common::PackProtoMsg(buffer, cs_msg)) return true;

    // send
    ConnectionSend(socket, buffer);

    // check and close connection
    if (close_connection) {
        _sock_map.erase(socket);
        ConnectionClose(socket);
    }

    return true;
}

// vim:ts=4:sw=4:et:ft=cpp:
