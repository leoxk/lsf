// File:        connect_config_service.cpp
// Description: ---
// Notes:       ---
// Author:      leoxiang <leoxiang727@qq.com>
// Revision:    2015-06-17 by leoxiang

#include "svr/common/connect_config_service.h"
#include "svr/common/common_header.h"
#include "svr/common/common_func.h"
#include "svr/common/common_proto.h"

bool ConnectConfigService::OnConnectionCreate(lsf::asio::Socket socket)
{
    // construct message
    msg::SS message;
    message.set_type(msg::SS_GET_DEPLOY_CONFIG_REQ);
    message.mutable_get_deploy_config_req()->set_server_type(_pserver->GetServerType());
    message.mutable_get_deploy_config_req()->set_server_id(_pserver->GetServerId());

    // send and recv
    if (!common::SendAndRecv(socket, message))
    {
        socket.Close();
        return false;
    }

    // check ret
    if (!message.get_deploy_config_rsp().result())
    {
        LSF_LOG_ERR("get config failed");
        return false;
    }
     
    // set server config
    _pserver->SetServerConfig(message.get_deploy_config_rsp().config());

    return true;
}

void ConnectConfigService::SetConfigServerAddress(std::string const & address)
{
    _service_config.add_connect_address(address);
    _service_config.set_service_type(_service_type);
}

// vim:ts=4:sw=4:et:ft=cpp:
