// File:        client_msg_service.cpp
// Description: ---
// Notes:       ---
// Author:      leoxiang <leoxiang727@qq.com>
// Revision:    2015-06-23 by leoxiang

#include "svr/connsvrd/client_msg_service.h"

bool AcceptClientMsgService::OnConnectionCreate(lsf::asio::Socket socket)
{
    return true;
}

bool AcceptClientMsgService::OnConnectionMessage(lsf::asio::Socket socket, std::string& message)
{
    return true;
}

bool AcceptClientMsgService::OnConnectionPeerClose(lsf::asio::Socket socket)
{
    return true;
}

// vim:ts=4:sw=4:et:ft=cpp:
