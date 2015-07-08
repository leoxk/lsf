// File:        common_service.cpp
// Description: ---
// Notes:       ---
// Author:      leoxiang <leoxiang727@qq.com>
// Revision:    2015-06-17 by leoxiang

#include "svr/common/common_service.h"
#include "svr/common/common_header.h"
#include "svr/common/common_func.h"
#include "svr/common/common_proto.h"
#include "svr/common/session_manager.h"
#include "svr/common/basic_handler.h"

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
    msg::CS cs_msg;
    if (!common::UnPackProtoMsg(message, cs_msg)) return true;

    // check new connection
    if (cs_msg.conn_head().is_new_conn()) {
        if (!OnClientConnectionCreate(cs_msg)) return false;
    }

    // check close connection
    if (cs_msg.conn_head().is_close_conn()) {
        OnClientConnectionPeerClose(cs_msg);
    }

    // just ignore empty message
    if (!cs_msg.has_msg_id()) return true;

    // create session and process
    Session* psession = SessionManager::Instance()->CreateSession();
    if (psession) {
        psession->set_session_type(cs_msg.msg_id());
        psession->set_session_state(data::SESSION_STATE_ON_CLIENT_REQ);
        psession->mutable_cs_request()->CopyFrom(cs_msg);
        HandlerManager::Instance()->ProcessSession(*psession);
    }

    return true;
}

////////////////////////////////////////////////////////////
// ConnectServerMsgTransferService
bool ConnectServerMsgTransferService::OnConnectionMessage(lsf::asio::Socket socket, std::string & message) {
    // unpack message
    msg::SS ss_msg;
    if (!common::UnPackProtoMsg(message, ss_msg)) return true;

    // just ignore empty message
    if (!ss_msg.has_msg_id()) return true;

    // check proxy transfer
    if (ss_msg.proxy_head().dst_server_type() != _pserver->GetServerType()) {
        LSF_LOG_FATAL("server type not match, type1=%u, type2=%u",
                ss_msg.proxy_head().dst_server_type(), _pserver->GetServerType());
        return true;
    }

    if (HandlerManager::Instance()->IsRequest(ss_msg.msg_id())) {
        // create session and process
        Session* psession = SessionManager::Instance()->CreateSession();
        if (psession) {
            psession->set_session_type(ss_msg.msg_id());
            psession->set_session_state(data::SESSION_STATE_ON_SERVER_REQ);
            psession->mutable_ss_request()->CopyFrom(ss_msg);
            HandlerManager::Instance()->ProcessSession(*psession);
        }
    }
    else {
        // restore session and process
        Session* psession = SessionManager::Instance()->GetSession(ss_msg.proxy_head().session_id());
        if (!psession) {
            LSF_LOG_FATAL("get session failed, session_id=%u", ss_msg.proxy_head().session_id());
            return true;
        }

        psession->mutable_ss_response()->CopyFrom(ss_msg);
        HandlerManager::Instance()->ProcessSessionResponse(*psession);
    }

    return true;
}


// vim:ts=4:sw=4:et:ft=cpp:
