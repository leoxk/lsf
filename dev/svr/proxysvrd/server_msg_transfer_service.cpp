// File:        server_msg_transfer_service.cpp
// Description: ---
// Notes:       ---
// Author:      leoxiang <leoxiang727@qq.com>
// Revision:    2015-06-26 by leoxiang

#include "svr/proxysvrd/server_msg_transfer_service.h"
#include "svr/common/common_header.h"
#include "svr/common/common_func.h"
#include "svr/common/common_service.h"

using namespace lsf::asio;
using namespace lsf::util;
using namespace google::protobuf;

bool AcceptServerMsgTransferService::InitSocketMapFromServerConfig() {
    // get all server config
    RepeatedPtrField<conf::Server> all_config;
    if (!ConnectConfigService::Instance()->GetAllConfig(all_config)) return false;

    // init socket map based on config
    for (auto const & conf : all_config) {
        // if not enough list then insert
        while (conf.server_type() >= _sock_map.size()) {
            _sock_map.push_back(sock_list_type());
        }

        // if not enough socket then insert
        sock_list_type & sock_list = _sock_map[conf.server_type()];
        while (conf.server_id() >= sock_list.size()) {
            sock_list.push_back(Socket());
        }
    }

    return true;
}

bool AcceptServerMsgTransferService::OnConnectionMessage(lsf::asio::Socket socket, std::string& message) {
    // unpack message
    msg::SS request;
    if (!common::UnPackProtoMsg(message, request)) return true;

    // choose handler
    switch (request.msg_id()) {
        case msg::SS::kRegisterToProxyReqFieldNumber:
            HandleRegisterRequest(socket, request);
            break;

        default:
            HandleMessageTransfer(socket, request, message);
            break;
    }

    return true;
}

void AcceptServerMsgTransferService::OnConnectionClose(lsf::asio::Socket socket) {
    // find and clear entery
    for (auto & sock_list : _sock_map) {
        for (auto & sock : sock_list) {
            if (sock == socket) {
                sock.SetSockFd(-1);
                return;
            }
        }
    }

    LSF_LOG_FATAL("connection close but cant find socket, fd=%d", socket.GetSockFd());
}

bool AcceptServerMsgTransferService::HandleRegisterRequest(lsf::asio::Socket socket, msg::SS const & request) {
    // check input
    size_t server_type = request.register_to_proxy_req().server_type();
    size_t server_id = request.register_to_proxy_req().server_id();
    if (server_type >= _sock_map.size()) {
        LSF_LOG_ERR("server type not in socket map, cur_val=%u, max_val=%u", server_type, _sock_map.size());
        return false;
    }
    if (server_id >= _sock_map[server_type].size()) {
        LSF_LOG_ERR("server id not in socket map, cur_val=%u, max_val=%u", server_id, _sock_map[server_type].size());
        return false;
    }

    // if connection already exist, disconn first
    Socket & map_socket = _sock_map[server_type][server_id];
    if (socket) {
        LSF_LOG_ERR("server register when socket already exist, server_type=%u, server_id=%u",
                server_type, server_id);
        ConnectionClose(map_socket);
    }

    // assign socket
    map_socket = socket;
    LSF_LOG_INFO("server register, server_type=%u, server_id=%u, address=%s",
            server_type, server_id, socket.RemoteSockAddr().ToCharStr());

    return true;
}

bool AcceptServerMsgTransferService::HandleMessageTransfer(lsf::asio::Socket socket, msg::SS const & request, std::string const & message) {
    // check proxy head
    if (!request.has_proxy_head()) {
        LSF_LOG_ERR("transfer not contain proxy head, msg_id=%u", request.msg_id());
        return false;
    }
    conf::ENServerType server_type         = request.proxy_head().dst_server_type();
    uint32_t server_id                     = request.proxy_head().dst_server_id();
    uint64_t hash_id                       = request.proxy_head().dst_hash_id();
    msg::ENProxyTransferType transfer_type = request.proxy_head().transfer_type();

    // check input
    if (server_type >= _sock_map.size()) {
        LSF_LOG_ERR("server type not in socket map, cur_val=%u, max_val=%u", server_type, _sock_map.size());
        return false;
    }
    sock_list_type & sock_list = _sock_map[server_type];

    // direct send
    if (transfer_type == msg::PROXY_TRANSFER_TYPE_BY_ID) {
        TransferMessageByTypeAndId(server_type, server_id, message);
    }

    // calc server id by hash, and send
    if (transfer_type == msg::PROXY_TRANSFER_TYPE_BY_HASH) {
        if (sock_list.empty()) return false;
        uint32_t server_id = hash_id % sock_list.size();
        TransferMessageByTypeAndId(server_type, server_id, message);
    }

    // travers all socket in same type, and send
    if (transfer_type == msg::PROXY_TRANSFER_TYPE_BY_BROADCAST) {
        for (uint32_t i = 0; i < sock_list.size(); ++i) {
            if (sock_list[i]) {
                TransferMessageByTypeAndId(server_type, i, message);
            }
        }
    }

    // random get a usable socket, and send
    if (transfer_type == msg::PROXY_TRANSFER_TYPE_BY_RANDOM) {
        uint32_t rand = SingleRandom::Instance()->GetRand(0, sock_list.size() - 1);
        for (uint32_t i = 0; i < sock_list.size(); ++i) {
            uint32_t index = (i + rand) % sock_list.size();
            if (sock_list[index]) {
                TransferMessageByTypeAndId(server_type, index, message);
                break;
            }
        }
    }

    return true;
}

bool AcceptServerMsgTransferService::TransferMessageByTypeAndId(conf::ENServerType server_type, uint32_t server_id, std::string const & message) {
    // check input
    if (server_type >= _sock_map.size()) {
        LSF_LOG_ERR("server type not in socket map, cur_val=%u, max_val=%u", server_type, _sock_map.size());
        return false;
    }
    if (server_id >= _sock_map[server_type].size()) {
        LSF_LOG_ERR("server id not in socket map, cur_val=%u, max_val=%u", server_id, _sock_map[server_type].size());
        return false;
    }

    // check socket
    Socket socket = _sock_map[server_type][server_id];
    if (!socket) {
        LSF_LOG_ERR("transfer server msg when not registered, server_type=%u, server_id=%u", server_type, server_id);
        return false;
    }

    // send message
    ConnectionSend(socket, message);
    return true;
}

// vim:ts=4:sw=4:et:ft=cpp:
