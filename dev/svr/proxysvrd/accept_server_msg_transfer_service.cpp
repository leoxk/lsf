// File:        accept_server_msg_transfer_service.cpp
// Description: ---
// Notes:       ---
// Author:      leoxiang <leoxiang727@qq.com>
// Revision:    2015-06-26 by leoxiang

#include "svr/proxysvrd/accept_server_msg_transfer_service.h"
#include "svr/common/common_header.h"
#include "svr/common/common_func.h"
#include "svr/common/common_service.h"

using namespace lsf::asio;
using namespace lsf::util;
using namespace google::protobuf;

bool AcceptServerMsgTransferService::InitSocketMapFromServerConfig() {
    // get all server config
    RepeatedPtrField<conf::Server> configs;
    if (!ConnectConfigService::Instance()->GetAllServerConfig(configs)) return false;

    // init socket map based on config
    for (auto& conf : configs) {
        // if not enough list then insert
        while (conf.server_type() >= _sock_map.size()) {
            _sock_map.push_back(sock_list_type());
        }

        // if not enough socket then insert
        sock_list_type& sock_list = _sock_map[conf.server_type()];
        while (conf.server_id() >= sock_list.size()) {
            sock_list.push_back(SharedSocket());
        }
    }

    return true;
}

void AcceptServerMsgTransferService::OnConnectionMessage(lsf::asio::SharedSocket socket, std::string& message) {
    // unpack message
    msg::SS request;
    if (!Protobuf::ParseProtoMsgFromString(message, request)) {
        LSF_LOG_ERR("parse failed, %s", LSF_PROTOBUF);
        return;
    }

    // choose handler
    switch (request.msg_type()) {
        case msg::SS_TYPE_REGISTER_TO_PROXY_REQ: HandleRegisterRequest(socket, request); break;
        default:                                 HandleMessageTransfer(socket, request, message); break;
    }
}

void  AcceptServerMsgTransferService::HandleRegisterRequest(lsf::asio::SharedSocket socket, msg::SS const& request) {
    // check input
    conf::ENServerType server_type = request.register_to_proxy_req().server_type();
    uint32_t server_id = request.register_to_proxy_req().server_id();
    if (server_type >= _sock_map.size()) {
        LSF_LOG_ERR("server type not in socket map, cur_val=%s, max_val=%u", LSF_ETS(server_type), _sock_map.size());
        return;
    }
    if (server_id >= _sock_map[server_type].size()) {
        LSF_LOG_ERR("server id not in socket map, cur_val=%u, max_val=%u", server_id, _sock_map[server_type].size());
        return;
    }

    // if connection already exist, disconn first
    auto& map_socket = _sock_map[server_type][server_id];
    if (map_socket && map_socket->IsConnect()) {
        LSF_LOG_FAT("server register when socket already exist, server_type=%s, server_id=%u, sockfd=%d",
                LSF_ETS(server_type), server_id, map_socket->SockFd());
        ConnectionClose(map_socket);
    }

    // assign socket
    map_socket = socket;
    LSF_LOG_INF("server register, server_type=%s, server_id=%u, address=%s",
            LSF_ETS(server_type), server_id, LSF_TS(socket->RemoteSockAddr()));
}

void AcceptServerMsgTransferService::HandleMessageTransfer(lsf::asio::SharedSocket socket, msg::SS const& request, std::string const& message) {
    // check proxy head
    if (!request.has_proxy_head()) {
        LSF_LOG_ERR("transfer not contain proxy head, msg_type=%s", LSF_ETS(request.msg_type()));
        return;
    }
    auto server_type   = request.proxy_head().dst_server_type();
    auto server_id     = request.proxy_head().dst_server_id();
    auto hash_id       = request.proxy_head().dst_hash_id();
    auto transfer_type = request.proxy_head().transfer_type();

    // check input
    if (server_type >= _sock_map.size()) {
        LSF_LOG_ERR("server type not in socket map, cur_val=%s, max_val=%u", LSF_ETS(server_type), _sock_map.size());
        return;
    }
    auto& sock_list = _sock_map[server_type];

    // direct send
    if (transfer_type == msg::PROXY_TRANSFER_TYPE_BY_ID) {
        TransferMessageByTypeAndId(server_type, server_id, request.proxy_head(), message);
    }

    // calc server id by hash, and send
    if (transfer_type == msg::PROXY_TRANSFER_TYPE_BY_HASH) {
        if (sock_list.empty()) return;
        uint32_t server_id = hash_id % sock_list.size();
        TransferMessageByTypeAndId(server_type, server_id, request.proxy_head(), message);
    }

    // travers all socket in same type, and send
    if (transfer_type == msg::PROXY_TRANSFER_TYPE_BY_BROADCAST) {
        for (uint32_t i = 0; i < sock_list.size(); ++i) {
            if (sock_list[i]) {
                TransferMessageByTypeAndId(server_type, i, request.proxy_head(), message);
            }
        }
    }

    // random get a usable socket, and send
    if (transfer_type == msg::PROXY_TRANSFER_TYPE_BY_RANDOM) {
        uint32_t rand = SingleRandom::Instance()->GetRand(0, sock_list.size() - 1);
        for (uint32_t i = 0; i < sock_list.size(); ++i) {
            uint32_t index = (i + rand) % sock_list.size();
            if (sock_list[index]) {
                TransferMessageByTypeAndId(server_type, index, request.proxy_head(), message);
                break;
            }
        }
    }

    if(transfer_type == msg::PROXY_TRANSFER_TYPE_BY_RANDOM_EXCEPT_ID) {
        auto bsend = false;
        auto except_id = request.proxy_head().dst_except_id();
        auto rand = SingleRandom::Instance()->GetRand(0, sock_list.size() - 1);
        for(uint32_t i = 0; i < sock_list.size(); ++i) {
            auto index = (i + rand) % sock_list.size();
            if(sock_list[index] && index != except_id) {
                TransferMessageByTypeAndId(server_type, index, request.proxy_head(), message);
                bsend = true;
                break;
            }
        }

        if(!bsend && sock_list[except_id]) {
            TransferMessageByTypeAndId(server_type, except_id, request.proxy_head(), message);
        }
    }
}

bool AcceptServerMsgTransferService::TransferMessageByTypeAndId(conf::ENServerType server_type, uint32_t server_id, msg::ProxyHead const& proxy_head, std::string const& message) {
    // check input
    if (server_type >= _sock_map.size()) {
        LSF_LOG_ERR("server type not in socket map, cur_val=%s, max_val=%u", LSF_ETS(server_type), _sock_map.size());
        return false;
    }
    if (server_id >= _sock_map[server_type].size()) {
        LSF_LOG_ERR("server id not in socket map, cur_val=%u, max_val=%u", server_id, _sock_map[server_type].size());
        return false;
    }

    // check socket
    auto socket = _sock_map[server_type][server_id];
    if (!socket || !socket->IsConnect()) {
        LSF_LOG_ERR("transfer server msg when not registered, server_type=%s, server_id=%u", LSF_ETS(server_type), server_id);
        return false;
    }

    // send message
    ConnectionSend(socket, message);

    LSF_LOG_INF("transfer message from[%s:%u] to[%s:%u], length=%u",
            LSF_ETS(proxy_head.src_server_type()), proxy_head.src_server_id(),
            LSF_ETS(server_type), server_id, message.size());
    return true;
}

// vim:ts=4:sw=4:et:ft=cpp:
