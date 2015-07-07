// File:        common_service.cpp
// Description: ---
// Notes:       ---
// Author:      leoxiang <leoxiang727@qq.com>
// Revision:    2015-06-17 by leoxiang

#include "svr/common/common_service.h"
#include "svr/common/common_header.h"
#include "svr/common/common_func.h"
#include "svr/common/common_proto.h"

////////////////////////////////////////////////////////////
// ConnectConfigService
void ConnectConfigService::SetServiceConfig(std::string const& address) {
    _service_config.add_connect_address(address);
    _service_config.set_service_type(_service_type);
}

bool ConnectConfigService::GetServerConfig(conf::Server & config) {
    // construct message
    msg::SS message;
    message. set_msg_id(msg::SS::kGetDeployConfigReqFieldNumber);
    message.mutable_get_deploy_config_req()->set_server_type(_pserver->GetServerType());
    message.mutable_get_deploy_config_req()->set_server_id(_pserver->GetServerId());

    // send and recv
    if (!common::SendAndRecv(_conn_scok[0], message)) return false;

    // check ret
    if (message.msg_id() != msg::SS::kGetDeployConfigRspFieldNumber || !message.get_deploy_config_rsp().result()) {
        LSF_LOG_ERR("get config failed");
        return false;
    }

    // return config
    config.CopyFrom(message.get_deploy_config_rsp().config());

    return true;
}

bool ConnectConfigService::GetAllConfig(google::protobuf::RepeatedPtrField<conf::Server> & all_config) {
    // construct message
    msg::SS message;
    message.set_msg_id(msg::SS::kGetAllDeployConfigReqFieldNumber);
    message.mutable_get_all_deploy_config_req();

    // send and recv
    if (!common::SendAndRecv(_conn_scok[0], message)) return false;

    // check ret
    if (message.msg_id() != msg::SS::kGetAllDeployConfigRspFieldNumber) {
        LSF_LOG_ERR("get all config failed");
        return false;
    }

    // return config
    all_config.CopyFrom(message.get_all_deploy_config_rsp().all_config());

    return true;
}

////////////////////////////////////////////////////////////
// ConnectClientMsgTransferService
bool ConnectClientMsgTransferService::OnConnectionMessage(lsf::asio::Socket socket, std::string & message) {
    // unpack message
    msg::CS request;
    if (!common::UnPackProtoMsg(message, request)) return true;

    // create session
    return true;
    // TODO
    return true;
}

// vim:ts=4:sw=4:et:ft=cpp:
