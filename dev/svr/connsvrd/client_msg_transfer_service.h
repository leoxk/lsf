// File:        client_msg_transfer_service.h
// Description: ---
// Notes:       ---
// Author:      leoxiang <leoxiang727@qq.com>
// Revision:    2015-06-23 by leoxiang

#pragma once

#include "lsf/basic/singleton.hpp"
#include "svr/common/basic_service.h"
#include "svr/common/common_proto.h"

class AcceptClientMsgTransferService : public BasicAcceptService, public lsf::basic::Singleton<AcceptClientMsgTransferService> {
public:
    AcceptClientMsgTransferService() : BasicAcceptService(conf::SERVICE_TYPE_CLIENT_MSG_TRANSFER) { }

    bool TransferMessage(std::string const & message);

protected:
    virtual bool OnConnectionCreate(lsf::asio::Socket socket);
    virtual void OnConnectionClose(lsf::asio::Socket socket);
    virtual bool OnConnectionMessage(lsf::asio::Socket socket, std::string& message);

protected:
    lsf::asio::Socket _socket;
};

// vim:ts=4:sw=4:et:ft=cpp:
