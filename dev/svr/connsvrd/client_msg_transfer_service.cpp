// File:        client_msg_transfer_service.cpp
// Description: ---
// Notes:       ---
// Author:      leoxiang <leoxiang727@qq.com>
// Revision:    2015-06-23 by leoxiang

#include "svr/connsvrd/client_msg_transfer_service.h"
#include "svr/common/common_header.h"
#include "svr/common/common_func.h"
#include "svr/connsvrd/client_msg_service.h"

using namespace lsf::asio;

bool AcceptClientMsgTransferService::OnConnectionCreate(lsf::asio::Socket socket) {
    // close old connection
    if (_socket) ConnectionClose(_socket);

    // set new connection
    _socket = socket;

    return true;
}

void AcceptClientMsgTransferService::OnConnectionClose(lsf::asio::Socket socket) {
    if (_socket == socket) _socket.Clear();
}

bool AcceptClientMsgTransferService::OnConnectionMessage(lsf::asio::Socket socket, std::string& message) {
    // transfer
    AcceptClientMsgService::Instance()->SendResposeToClient(message);
    return true;
}

bool AcceptClientMsgTransferService::TransferMessage(google::protobuf::MessageLite const& proto_msg) {
    std::string message;
    if (!common::PackProtoMsg(message, proto_msg)) return false;
    return ConnectionSend(_socket, message);
}

// vim:ts=4:sw=4:et:ft=cpp:
