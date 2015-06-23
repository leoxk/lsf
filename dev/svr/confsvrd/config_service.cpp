// File:        config_service.cpp
// Description: ---
// Notes:       ---
// Author:      leoxiang <leoxiang727@qq.com>
// Revision:    2015-06-15 by leoxiang

#include "svr/common/common_header.h"
#include "svr/common/common_func.h"
#include "svr/confsvrd/config_service.h"
#include "svr/confsvrd/deploy_conf_mng.h"
#include "svr/proto/msg_ss.pb.h"

bool AcceptConfigService::OnConnectionMessage(lsf::asio::Socket socket, std::string& message) {
    msg::SS request;
    msg::SS response;
    response.set_type(msg::SS_GET_DEPLOY_CONFIG_RSP);
    response.mutable_get_deploy_config_rsp()->set_result(false);

    if (!common::UnPackProtoMsg(message, request)) return true;

    // check msg type
    if (request.type() != msg::SS_GET_DEPLOY_CONFIG_REQ) {
        common::SendMessage(socket, response);
        LSF_LOG_ERR("unknown message type, type=%u", request.type());
        return true;
    }

    // get config
    conf::ENServerType server_type = request.get_deploy_config_req().server_type();
    uint32_t server_id = request.get_deploy_config_req().server_id();
    conf::Server const* pconf = DeployConfigManager::Instance()->GetServerConfig(server_type, server_id);
    if (!pconf) {
        common::SendMessage(socket, response);
        LSF_LOG_ERR("cant find deploy config, server_type=%u, server_id=%u", server_type, server_id);
        return true;
    }

    // send response
    response.set_type(msg::SS_GET_DEPLOY_CONFIG_RSP);
    response.mutable_get_deploy_config_rsp()->set_result(true);
    response.mutable_get_deploy_config_rsp()->mutable_config()->CopyFrom(*pconf);
    common::SendMessage(socket, response);

    return true;
}

// vim:ts=4:sw=4:et:ft=cpp:
