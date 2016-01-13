// File:        accept_config_service.cpp
// Description: ---
// Notes:       ---
// Author:      leoxiang <leoxiang727@qq.com>
// Revision:    2015-06-15 by leoxiang

#include "svr/common/common_header.h"
#include "svr/common/common_func.h"
#include "svr/common/common_proto.h"
#include "svr/common/config_manager.h"
#include "svr/confsvrd/accept_config_service.h"
#include "svr/confsvrd/config_center.h"
#include "svr/confsvrd/config_server.h"

using namespace lsf::util;

void AcceptConfigService::OnConnectionMessage(lsf::asio::SharedSocket socket, std::string& message) {
    // unpack message
    msg::SS request;
    if (!Protobuf::ParseProtoMsgFromString(message, request)) {
        LSF_LOG_ERR("parse failed, %s", LSF_PROTOBUF);
        return;
    }

    // choose handler
    switch (request.msg_type()) {
        case msg::SS_TYPE_GET_SERVER_CONFIG_REQ: HandleGetServerConfig(socket, request); break;
        case msg::SS_TYPE_GET_ALL_SERVER_CONFIG_REQ: HandleGetAllServerConfig(socket, request); break;
        case msg::SS_TYPE_GET_GAME_CONFIG_REQ: HandleGetGameConfig(socket, request); break;
        default: LSF_LOG_ERR("unknow message id, %s", LSF_ETS(request.msg_type()));
    }
}

void AcceptConfigService::HandleGetServerConfig(lsf::asio::SharedSocket socket, msg::SS const & request) {
    msg::SS response;
    response.set_msg_type(msg::SS_TYPE_GET_SERVER_CONFIG_RSP);

    // get config
    conf::ENServerType server_type = request.get_server_config_req().server_type();
    uint32_t server_id = request.get_server_config_req().server_id();
    if (!ConfigCenter::Instance()->GetServerConfig(*response.mutable_get_server_config_rsp()->mutable_config(), server_type, server_id)) {
        response.mutable_get_server_config_rsp()->set_result(msg::INTERNAL_ERROR_CANT_FIND_CONFIG);
        ConnectionSend(socket, response);
        LSF_LOG_ERR("cant find server config, server_type=%s, server_id=%u", LSF_ETS(server_type), server_id);
        return;
    }

    // send response
    ConnectionSend(socket, response);
}

void AcceptConfigService::HandleGetAllServerConfig(lsf::asio::SharedSocket socket, msg::SS const & request) {
    msg::SS response;
    response.set_msg_type(msg::SS_TYPE_GET_ALL_SERVER_CONFIG_RSP);

    // get config
    response.mutable_get_all_server_config_rsp()->mutable_configs()->CopyFrom(
            ConfigCenter::Instance()->DeployConfig().server_configs());

    // send response
    ConnectionSend(socket, response);
}

void AcceptConfigService::HandleGetGameConfig(lsf::asio::SharedSocket socket, msg::SS const & request) {
    msg::SS response;
    response.set_msg_type(msg::SS_TYPE_GET_GAME_CONFIG_RSP);

    // get config
    conf::ENServerType server_type = request.get_game_config_req().server_type();
    uint32_t server_id = request.get_game_config_req().server_id();
    if (!ConfigCenter::Instance()->GetGameConfig(*response.mutable_get_game_config_rsp()->mutable_config(), server_type, server_id)) {
        response.mutable_get_game_config_rsp()->set_result(msg::INTERNAL_ERROR_CANT_FIND_CONFIG);
        ConnectionSend(socket, response);
        LSF_LOG_ERR("cant find game config, server_type=%s, server_id=%u", LSF_ETS(server_type), server_id);
        return;
    }

    // send response
    ConnectionSend(socket, response);
}

void AcceptConfigService::OnConnectionCreate(lsf::asio::SharedSocket socket){
    _sock_map[socket->SockFd()] = socket;
}

void AcceptConfigService::ReleaseAllConnections(){
    for(auto& pair : _sock_map) {
        ConnectionClose(pair.second);
    }

    _sock_map.clear();
}


// vim:ts=4:sw=4:et:ft=cpp:
