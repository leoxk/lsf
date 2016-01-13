// File:        accept_client_msg_transfer_service.h
// Description: ---
// Notes:       ---
// Author:      leoxiang <leoxiang727@qq.com>
// Revision:    2015-06-23 by leoxiang

#pragma once

#include "lsf/basic/singleton.hpp"
#include "svr/common/common_proto.h"
#include "svr/common/basic_service.h"

class AcceptClientMsgTransferService : public BasicAcceptService, public lsf::basic::Singleton<AcceptClientMsgTransferService> {
public:
    static const size_t DEF_SAVE_UNSEND_DATA_TIME = 60*1000;

public:
    AcceptClientMsgTransferService() : BasicAcceptService(conf::SERVICE_TYPE_CLIENT_MSG_TRANSFER) { }
    void TransferMessage(google::protobuf::Message const& message);

protected:
    virtual bool OnRun();
    virtual void OnConnectionCreate(lsf::asio::SharedSocket socket);
    virtual void OnConnectionMessage(lsf::asio::SharedSocket socket, std::string& message);

protected:
    lsf::asio::SharedSocket _socket;
    msg::ConnSvrInfo _conn_svr_info;
    std::string _unsend_data;
    bool _save_unsend_data = false;
};

// vim:ts=4:sw=4:et:ft=cpp:
