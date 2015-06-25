// File:        cmsg_transfer_service.cpp
// Description: ---
// Notes:       ---
// Author:      leoxiang <leoxiang727@qq.com>
// Revision:    2015-06-23 by leoxiang

#include "svr/connsvrd/cmsg_transfer_service.h"
#include "svr/common/common_header.h"
#include "svr/common/common_func.h"
#include "svr/connsvrd/client_msg_service.h"

using namespace lsf::asio;

bool AcceptClientMsgTransferService::OnConnectionCreate(lsf::asio::Socket socket) {
    // close old connection
    if (_socket.operator!()) ConnectionClose(_socket);

    // set new connection
    _socket = socket;

    return true;
}

bool AcceptClientMsgTransferService::OnConnectionMessage(lsf::asio::Socket socket, std::string& message) {
    // transfer
    AcceptClientMsgService::Instance()->SendResposeToClient(message);
    return true;
}

bool AcceptClientMsgTransferService::OnConnectionPeerClose(lsf::asio::Socket socket) {
    // close connection
    ConnectionClose(_socket);
    return true;
}

bool AcceptClientMsgTransferService::TransferMessage(std::string const & message) {
    return ConnectionSend(_socket, message);
}

// vim:ts=4:sw=4:et:ft=cpp:
