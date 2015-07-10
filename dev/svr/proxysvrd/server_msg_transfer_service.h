// File:        server_msg_transfer_service.h
// Description: ---
// Notes:       ---
// Author:      leoxiang <leoxiang727@qq.com>
// Revision:    2015-06-26 by leoxiang

#pragma once

#include <unordered_map>
#include "lsf/basic/singleton.hpp"
#include "svr/common/basic_service.h"
#include "svr/common/common_proto.h"

class AcceptServerMsgTransferService : public BasicAcceptService, public lsf::basic::Singleton<AcceptServerMsgTransferService> {
public:
    using sock_list_type = std::vector<lsf::asio::Socket>;
    using sock_map_type = std::vector<sock_list_type>;

public:
    AcceptServerMsgTransferService() : BasicAcceptService(conf::SERVICE_TYPE_SERVER_MSG_TRANSFER) { }

    bool InitSocketMapFromServerConfig();
    bool HandleRegisterRequest(lsf::asio::Socket socket, msg::SS const & request);
    bool HandleMessageTransfer(lsf::asio::Socket socket, msg::SS const & request, std::string const & message);

    lsf::asio::Socket FindSocket(conf::ENServerType server_type, uint32_t server_id);

protected:
    virtual bool OnConnectionMessage(lsf::asio::Socket socket, std::string& message);
    virtual void OnConnectionClose(lsf::asio::Socket socket);

private:
    bool TransferMessageByTypeAndId(conf::ENServerType server_type, uint32_t server_id, std::string const & message);

protected:
    sock_map_type _sock_map;
};


// vim:ts=4:sw=4:et:ft=cpp:
