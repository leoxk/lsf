// File:        client_msg_transfer_service.cpp
// Description: ---
// Notes:       ---
// Author:      leoxiang <leoxiang727@qq.com>
// Revision:    2015-06-23 by leoxiang

#include "svr/common/common_header.h"
#include "svr/common/common_func.h"
#include "svr/common/config_manager.h"
#include "svr/connsvrd/accept_client_msg_transfer_service.h"
#include "svr/connsvrd/accept_client_msg_service.h"

using namespace lsf::asio;
using namespace lsf::util;
using namespace lsf::basic;

bool AcceptClientMsgTransferService::OnRun() {
    // init listen address
    if (ConfigManager::Instance()->ServerConfig().external_addresses_size() != 0) {
        for (auto& address : ConfigManager::Instance()->ServerConfig().external_addresses()) {
            auto* paddress = _conn_svr_info.add_listen_addresses();
            paddress->set_ip(StringExt::SplitGet(address, '|', 0));
            paddress->set_port(TypeCast<uint32_t>(StringExt::SplitGet(address, '|', 1)));
        }
    }
    else {
        for (auto& address : _service_config.listen_addresses()) {
            auto* paddress = _conn_svr_info.add_listen_addresses();
            paddress->set_ip(System::GetExternalAddress().IsAny() ?
                    System::GetInternalAddress().ToString() :
                    System::GetExternalAddress().ToString());
            paddress->set_port(TypeCast<uint32_t>(StringExt::SplitGet(address, '|', 1)));
        }
    }

    return true;
}

void AcceptClientMsgTransferService::OnConnectionCreate(lsf::asio::SharedSocket socket) {
    // close old connection
    if (_socket && _socket->IsConnect()) ConnectionClose(_socket);

    // set new connection
    _socket = socket;

    // first send connsvrinfo
    ConnectionSend(socket, _conn_svr_info);
}

void AcceptClientMsgTransferService::OnConnectionMessage(lsf::asio::SharedSocket socket, std::string& message) {
    // unpack
    msg::CS cs_msg;
    if (!Protobuf::ParseProtoMsgFromString(message, cs_msg)) {
        LSF_LOG_ERR("parse failed, %s", LSF_PROTOBUF);
        return;
    }

    // transfer by id
    if (cs_msg.conn_head().has_conn_id()) {
        AcceptClientMsgService::Instance()->SendMessageToClient(cs_msg);
    }

    // transfer by ids
    for (auto conn_id : cs_msg.conn_head().conn_ids()) {
        cs_msg.mutable_conn_head()->set_conn_id(conn_id);
        AcceptClientMsgService::Instance()->SendMessageToClient(cs_msg);
    }
}

void AcceptClientMsgTransferService::TransferMessage(google::protobuf::Message const& message) {
    // check connect
    if (!_socket || !_socket->IsConnect()) {
        LSF_LOG_ERR("transfer message but socket invalid");
        return;
    }

    // transfer content
    std::string content;
    if (!Protobuf::SerializeProtoMsgToString(content, message)) {
        LSF_LOG_ERR("serialize failed, %s", LSF_PROTOBUF);
        return;
    }

    // send
    if (!ConnectionSend(_socket, content) && _save_unsend_data) {
        _unsend_data.append(content);
    }
}

// vim:ts=4:sw=4:et:ft=cpp:
