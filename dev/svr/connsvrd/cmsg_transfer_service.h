// File:        cmsg_transfer_service.h
// Description: ---
// Notes:       ---
// Author:      leoxiang <leoxiang727@qq.com>
// Revision:    2015-06-23 by leoxiang

#pragma once

#include "lsf/basic/singleton.hpp"
#include "svr/common/basic_service.h"

class AcceptClientMsgTransferService : public BasicAcceptService, public lsf::basic::Singleton<AcceptClientMsgTransferService> {
public:
    AcceptClientMsgTransferService() : BasicAcceptService(conf::SERVICE_TYPE_CLIENT_MSG_TRANSFER) { }

private:
    virtual bool OnConnectionCreate(lsf::asio::Socket socket);
    virtual bool OnConnectionMessage(lsf::asio::Socket socket, std::string& message);
    virtual bool OnConnectionPeerClose(lsf::asio::Socket socket);
};

// vim:ts=4:sw=4:et:ft=cpp:
