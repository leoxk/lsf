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
    msg::TcpHead & tcp_head = _sock_map[socket];
    tcp_head.set_index(socket.GetSockFd());
    tcp_head.set_client_ip(socket.RemoteSockAddr().GetAddress().ToString());
    tcp_head.set_client_port(socket.RemoteSockAddr().GetPort());
    tcp_head.set_is_new_connection(true);
    tcp_head.set_is_close_connection(false);
    tcp_head.set_connect_time(IOService::Instance()->GetClockTime());

    return true;
}

bool AcceptClientMsgService::OnConnectionMessage(lsf::asio::Socket socket, std::string& message) {
    // unpack msg
    msg::CS request;
    if (!common::UnPackProtoMsg(message, request)) return true;

    // copy tcp head
    msg::TcpHead & head = _sock_map[socket];
    request.mutable_tcp_head()->CopyFrom(head);

    // clear new connection mark
    if (head.is_new_connection()) head.clear_is_new_connection();

    // pack msg
    std::string buffer;
    if (!common::PackProtoMsg(buffer, request)) return true;

    // transfer
    AcceptClientMsgTransferService::Instance()->TransferMessage(buffer);

    return true;
}

void AcceptClientMsgService::OnConnectionClose(lsf::asio::Socket socket) {
    // erase map entry
    _sock_map.erase(socket);
}

bool AcceptClientMsgService::SendResposeToClient(std::string &message) {
    // unpack
    msg::CS response;
    if (!common::UnPackProtoMsg(message, response)) return true;

    // get socket from head
    Socket socket = { response.tcp_head().index() };
    if (socket) {
        LSF_LOG_ERR("socket get from tcp head err, fd=%u", socket.GetSockFd());
        return true;
    }
    bool close_connection = response.tcp_head().is_close_connection();

    // pack new message
    std::string buffer;
    response.clear_tcp_head();
    if (!common::PackProtoMsg(buffer, response)) return true;

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
