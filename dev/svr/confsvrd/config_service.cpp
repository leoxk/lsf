// File:        config_service.cpp
// Description: ---
// Notes:       ---
// Author:      leoxiang <leoxiang727@qq.com>
// Revision:    2015-06-15 by leoxiang

#include "svr/common/common_header.h"
#include "svr/common/common_func.h"
#include "svr/confsvrd/config_service.h"
#include "svr/confsvrd/deploy_conf_manager.h"
#include "svr/proto/msg_ss.pb.h"

bool AcceptConfigService::OnConnectionMessage(lsf::asio::Socket socket, std::string& message) {
    // unpack message
    msg::SS request;
    if (!common::UnPackProtoMsg(message, request)) return true;

    // choose handler
    switch (request.msg_type()) {
        case msg::SS::kGetDeployConfigReqFieldNumber:
            HandleGetDeployConfig(socket, request);
            break;

        case msg::SS::kGetAllDeployConfigReqFieldNumber:
            HandleGetAllDeployConfig(socket, request);
            break;

        default:
            LSF_LOG_ERR("unknow message id, %u", request.msg_type());
    }

    return true;
}


bool AcceptConfigService::HandleGetDeployConfig(lsf::asio::Socket socket, msg::SS const & request) {
    msg::SS response;
    response.set_msg_type(msg::SS_TYPE_GET_DEPLOY_CONFIG_RSP);

    // get config
    conf::ENServerType server_type = request.get_deploy_config_req().server_type();
    uint32_t server_id = request.get_deploy_config_req().server_id();
    conf::Server const* pconf = DeployConfigManager::Instance()->GetServerConfig(server_type, server_id);
    if (!pconf) {
        response.mutable_get_deploy_config_rsp()->set_result(msg::INTERNAL_ERROR_CANT_FIND_CONFIG);
        ConnectionSend(socket, response);
        LSF_LOG_ERR("cant find deploy config, server_type=%u, server_id=%u", server_type, server_id);
        return true;
    }

    // send response
    response.mutable_get_deploy_config_rsp()->set_result(msg::INTERNAL_ERROR_OK);
    response.mutable_get_deploy_config_rsp()->mutable_config()->CopyFrom(*pconf);
    ConnectionSend(socket, response);

    return true;
}

bool AcceptConfigService::HandleGetAllDeployConfig(lsf::asio::Socket socket, msg::SS const & request) {
    msg::SS response;
   response.set_msg_type(msg::SS_TYPE_GET_ALL_DEPLOY_CONFIG_RSP);

    // get config
    response.mutable_get_all_deploy_config_rsp()->mutable_all_config()->CopyFrom(
            DeployConfigManager::Instance()->GetAllServerConfig());

    // send response
    ConnectionSend(socket, response);

    return true;
}

// vim:ts=4:sw=4:et:ft=cpp:
