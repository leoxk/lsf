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
#include "svr/common/handler_manager.h"

////////////////////////////////////////////////////////////
// ConnectConfigService
void ConnectConfigService::SetServiceConfig(std::string const& address) {
    _service_config.add_connect_address(address);
    _service_config.set_service_type(_service_type);
}

bool ConnectConfigService::GetServerConfig(conf::Server & config) {
    // construct message
    msg::SS message;
    message. set_msg_type(msg::SS_TYPE_GET_DEPLOY_CONFIG_REQ);
    message.mutable_get_deploy_config_req()->set_server_type(_pserver->GetServerType());
    message.mutable_get_deploy_config_req()->set_server_id(_pserver->GetServerId());

    // send and recv
    if (!common::SendAndRecv(_conn_scok[0], message)) return false;

    // check ret
    if (message.msg_type() != msg::SS::kGetDeployConfigRspFieldNumber ||
            message.get_deploy_config_rsp().result() != msg::INTERNAL_ERROR_OK) {
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
    message.set_msg_type(msg::SS_TYPE_GET_ALL_DEPLOY_CONFIG_REQ);
    message.mutable_get_all_deploy_config_req();

    // send and recv
    if (!common::SendAndRecv(_conn_scok[0], message)) return false;

    // check ret
    if (message.msg_type() != msg::SS::kGetAllDeployConfigRspFieldNumber) {
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
    if (!cs_msg.has_msg_type()) return true;

    // create session and process
    Session* psession = SessionManager::Instance()->CreateSession();
    if (psession) {
        psession->set_session_type(cs_msg.msg_type());
        psession->set_session_state(data::SESSION_STATE_ON_CLIENT_REQ);
        psession->mutable_cs_request()->CopyFrom(cs_msg);
        HandlerManager::Instance()->ProcessSession(*psession);
    }

    return true;
}

bool ConnectClientMsgTransferService::SendMessage(Session& session) {
    // set conn head and cs head
    session.mutable_cs_response()->mutable_conn_head()->CopyFrom(session.cs_request().conn_head());
    session.mutable_cs_response()->mutable_cs_head()->CopyFrom(session.cs_request().cs_head());

    // set msg id if needed
    if (!session.cs_response().has_msg_type()) session.mutable_cs_response()->set_msg_type(msg::CSType(session.cs_request().msg_type() + 1));

    // send
    return ConnectionSend(_conn_scok[0], session.cs_response());
}

////////////////////////////////////////////////////////////
// ConnectServerMsgTransferService
bool ConnectServerMsgTransferService::OnConnectionMessage(lsf::asio::Socket socket, std::string & message) {
    // unpack message
    msg::SS ss_msg;
    if (!common::UnPackProtoMsg(message, ss_msg)) return true;

    // just ignore empty message
    if (!ss_msg.has_msg_type()) return true;

    // check proxy transfer
    if (ss_msg.proxy_head().dst_server_type() != _pserver->GetServerType()) {
        LSF_LOG_FATAL("server type not match, type1=%u, type2=%u",
                ss_msg.proxy_head().dst_server_type(), _pserver->GetServerType());
        return true;
    }

    if (HandlerManager::Instance()->IsRequest(ss_msg.msg_type())) {
        // create session and process
        Session* psession = SessionManager::Instance()->CreateSession();
        if (psession) {
            psession->set_session_type(ss_msg.msg_type());
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

bool ConnectServerMsgTransferService::SendMessage(Session& session) {
    // set proxy head
    msg::ProxyHead& proxy_head = *session.mutable_ss_response()->mutable_proxy_head();
    proxy_head.CopyFrom(session.ss_request().proxy_head());
    proxy_head.set_dst_server_type(proxy_head.src_server_type());
    proxy_head.set_dst_server_id(proxy_head.src_server_id());
    proxy_head.set_src_server_type(_pserver->GetServerType());
    proxy_head.set_src_server_id(_pserver->GetServerId());

    // set msg id if needed
    if (!session.ss_response().has_msg_type()) session.mutable_ss_response()->set_msg_type(msg::SSType(session.ss_request().msg_type() + 1));

    // send
    return ConnectionSend(_conn_scok[0], session.ss_response());
}


// vim:ts=4:sw=4:et:ft=cpp:
