// File:        common_service.cpp
// Description: ---
// Notes:       ---
// Author:      leoxiang <leoxiang727@qq.com>
// Revision:    2015-06-17 by leoxiang

#include "svr/common/common_service.h"
#include "svr/common/common_header.h"
#include "svr/common/common_func.h"
#include "svr/common/common_proto.h"
#include "svr/common/pb2json.h"
#include "svr/common/session_manager.h"
#include "svr/common/basic_handler.h"
#include "svr/common/handler_manager.h"
#include "svr/common/config_manager.h"
#include "svr/common/client_conn_manager.h"

using namespace lsf::asio;
using namespace lsf::util;

////////////////////////////////////////////////////////////
// ConnectConfigService
void ConnectConfigService::OnConnectionCreate(lsf::asio::SharedSocket socket) {
	// init server config
	if (!InitServerConfig()) { ConnectionClose(socket); return; }
    if (!InitGameConfig()) { ConnectionClose(socket); return; }

	// check server type and id
	auto& server_config = ConfigManager::Instance()->ServerConfig();
	if (_pserver->GetServerType() != server_config.server_type() || _pserver->GetServerId() != server_config.server_id()) {
		LSF_LOG_ERR_STACK("server not match, input=%s %u, config=%s %u", LSF_ETS(_pserver->GetServerType()), _pserver->GetServerId(),
		LSF_ETS(server_config.server_type()), server_config.server_id());
		ConnectionClose(socket);
		return;
	}
}

void ConnectConfigService::SetConfigServiceAddress(std::string const& address) {
	_service_config.set_service_type(_service_type);
	_service_config.add_connect_addresses(address);
}

bool ConnectConfigService::GetAllServerConfig(google::protobuf::RepeatedPtrField<conf::Server>& configs) {
	// construct message
	msg::SS message;
	message.set_msg_type(msg::SS_TYPE_GET_ALL_SERVER_CONFIG_REQ);
	message.mutable_get_all_server_config_req();

	// send and recv
	if (!common::SendAndRecv(_conn_scok[0], message)) return false;

	// check ret
	if (message.msg_type() != msg::SS_TYPE_GET_ALL_SERVER_CONFIG_RSP) {
		LSF_LOG_ERR("get all server config failed");
		return false;
	}

	// return config
	configs.CopyFrom(message.get_all_server_config_rsp().configs());

	return true;
}

bool ConnectConfigService::InitServerConfig() {
	// construct message
	msg::SS message;
	message. set_msg_type(msg::SS_TYPE_GET_SERVER_CONFIG_REQ);
	message.mutable_get_server_config_req()->set_server_type(_pserver->GetServerType());
	message.mutable_get_server_config_req()->set_server_id(_pserver->GetServerId());

	// send and recv
	if (!common::SendAndRecv(_conn_scok[0], message)) return false;

	// check ret
	if (message.msg_type() != msg::SS_TYPE_GET_SERVER_CONFIG_RSP ||
			message.get_server_config_rsp().result() != msg::INTERNAL_ERROR_OK) {
		LSF_LOG_ERR("get server config failed");
		return false;
	}

	// assign config
	ConfigManager::Instance()->SetServerConfig(message.get_server_config_rsp().config());

    // callback
    for (auto& func : _deploy_config_reload_func_set) func();

	return true;
}

bool ConnectConfigService::InitGameConfig() {
	// construct message
	msg::SS message;
	message.set_msg_type(msg::SS_TYPE_GET_GAME_CONFIG_REQ);
	message.mutable_get_game_config_req()->set_server_type(_pserver->GetServerType());
	message.mutable_get_game_config_req()->set_server_id(_pserver->GetServerId());

	// send and recv
	if (!common::SendAndRecv(_conn_scok[0], message)) return false;

	// check ret
	if (message.msg_type() != msg::SS_TYPE_GET_GAME_CONFIG_RSP) {
		LSF_LOG_ERR("get game config failed");
		return false;
	}

	// set config
	ConfigManager::Instance()->SetGameConfig(message.get_game_config_rsp().config());

    // callback
    for (auto& func : _game_config_reload_func_set) func();

	return true;
}

////////////////////////////////////////////////////////////
// ConnectClientMsgTransferService
void ConnectClientMsgTransferService::OnConnectionCreate(lsf::asio::SharedSocket socket) {
    // receive connsvrinfo
    if (!common::BlockRecv(socket, _conn_svr_info)) {
        ConnectionClose(socket);
    }

    // print out address
    for (auto& address : _conn_svr_info.listen_addresses()) {
        LSF_LOG_INF("init conn svr address: %s|%u", address.ip().c_str(), address.port());
    }
}

void ConnectClientMsgTransferService::OnConnectionMessage(lsf::asio::SharedSocket socket, std::string& content) {
	// unpack content
	msg::CS message;
	if (!Protobuf::ParseProtoMsgFromString(content, message)) {
		LSF_LOG_ERR("parse failed, %s", LSF_PROTOBUF);
		return;
	}

	// check new connection
	if (message.conn_head().is_new_conn() && ClientConnManager::Instance()->IsInit()) {
		ClientConn* pconn = ClientConnManager::Instance()->CreateClientConn(message);
		if (pconn == nullptr) {
			// close this connection
			msg::CS message;
			message.mutable_conn_head()->CopyFrom(message.conn_head());
			ConnectionSendByIndex(0, message);
			return;
		}
	}

	// check close connection
	if (message.conn_head().is_close_conn() && ClientConnManager::Instance()->IsInit()) {
		ClientConnManager::Instance()->ReleaseClientConn(message.conn_head().conn_id());
        return;
	}

	// just ignore empty content
	if (!message.has_msg_type()) {
		LSF_LOG_ERR("content do not have msg type, msg_len=%u, %s", content.length(), LSF_PTS(message));
        return;
    }

	// create session and process
	Session* psession = SessionManager::Instance()->CreateSession();
	if (psession) HandlerManager::Instance(message)->GetClientRequest(*psession, message);
}

bool ConnectClientMsgTransferService::TransferResponse(Session& session) {
	// set conn head and cs head
	session.mutable_cs_response()->mutable_conn_head()->CopyFrom(session.cs_request().conn_head());

	// send
	LSF_LOG_INF("send response to client, %s, msg_type=%s", LSF_TS(session), LSF_ETS(session.cs_response().msg_type()));
	return ConnectionSendByIndex(0, session.cs_response());
}

bool ConnectClientMsgTransferService::TransferMessageByConnId(uint32_t conn_id, msg::CS& message) {
	// set conn head
	message.mutable_conn_head()->set_conn_id(conn_id);

	// send
	// LSF_LOG_INF("send notify to client, conn_id=%u, msg_type=%s", conn_id, LSF_ETS(message.msg_type()));
	return ConnectionSendByIndex(0, message);
}

bool ConnectClientMsgTransferService::TransferMessageByConnIds(google::protobuf::RepeatedField<uint32_t> const& conn_ids, msg::CS& message) {
	// set conn head
	message.mutable_conn_head()->mutable_conn_ids()->CopyFrom(conn_ids);

	// send
	// LSF_LOG_INF("send notify to client, conn_id=%u, msg_type=%s", conn_id, LSF_ETS(message.msg_type()));
	return ConnectionSendByIndex(0, message);
}

////////////////////////////////////////////////////////////
// ConnectServerMsgTransferService
bool ConnectServerMsgTransferService::OnRun() {
    // register timer handle
    TimerManager::Instance()->AddTimerHandle(data::TIMER_TYPE_ASYNC_INTERNAL_MESSAGE, [](Timer const& timer) {
            if (timer.has_internal_message() && HandlerManager::IsRequest(timer.internal_message().msg_type())) {
                auto& message = timer.internal_message();
                Session* psession = SessionManager::Instance()->CreateSession();
                if (psession) HandlerManager::Instance(message)->GetServerRequest(*psession, message);
            }
        });

    return true;
}

void ConnectServerMsgTransferService::OnConnectionCreate(lsf::asio::SharedSocket socket) {
	// send register message to proxy
	msg::SS message;
	message.set_msg_type(msg::SS_TYPE_REGISTER_TO_PROXY_REQ);
	message.mutable_register_to_proxy_req()->set_server_type(_pserver->GetServerType());
	message.mutable_register_to_proxy_req()->set_server_id(_pserver->GetServerId());
	ConnectionSend(socket, message);
}

void ConnectServerMsgTransferService::OnConnectionMessage(lsf::asio::SharedSocket socket, std::string & content) {
	// unpack content
	msg::SS message;
	if (!Protobuf::ParseProtoMsgFromString(content, message)) {
		LSF_LOG_ERR("parse failed, %s", LSF_PROTOBUF);
		return;
	}

	// just ignore empty content
	if (!message.has_msg_type()) {
		LSF_LOG_ERR("content do not have msg type, msg_len=%u, %s", content.length(), LSF_PTS(message));
		return;
	}

	// check proxy transfer
	if (message.proxy_head().dst_server_type() != _pserver->GetServerType()) {
		LSF_LOG_FAT("server type not match, type1=%u, type2=%u",
		message.proxy_head().dst_server_type(), _pserver->GetServerType());
		return;
	}

	if (HandlerManager::IsRequest(message.msg_type())) {
		// create session and process
		Session* psession = SessionManager::Instance()->CreateSession();
		if (psession) HandlerManager::Instance(message)->GetServerRequest(*psession, message);
	}
	else {
		// restore session and process
		Session* psession = SessionManager::Instance()->GetSession(message.proxy_head().session_id());
		if (!psession) {
			LSF_LOG_FAT("get session failed, msg_type=%s, session_id=%u",
			LSF_ETS(message.msg_type()), message.proxy_head().session_id());
			return;
		}
		HandlerManager::Instance(*psession)->GetServerResponse(*psession, message);
	}
}

void ConnectServerMsgTransferService::PostInternalMessage(msg::SS& message) {
	if (HandlerManager::IsRequest(message.msg_type())) {
		// create session and process
		Session* psession = SessionManager::Instance()->CreateSession();
		if (psession) HandlerManager::Instance(message)->GetServerRequest(*psession, message);
	}
}

void ConnectServerMsgTransferService::PostAsyncInternalMessage(msg::SS& message) {
    auto* ptimer = TimerManager::Instance()->CreateTimer(0, data::TIMER_TYPE_ASYNC_INTERNAL_MESSAGE);
    if (ptimer) {
        ptimer->mutable_internal_message()->CopyFrom(message);
    }
}

bool ConnectServerMsgTransferService::TransferResponse(Session& session) {
	// set proxy head
	msg::ProxyHead& proxy_head = *session.mutable_ss_response()->mutable_proxy_head();
	proxy_head.CopyFrom(session.ss_request().proxy_head());
	proxy_head.set_transfer_type(msg::PROXY_TRANSFER_TYPE_BY_ID);
	proxy_head.set_dst_server_type(proxy_head.src_server_type());
	proxy_head.set_dst_server_id(proxy_head.src_server_id());
	proxy_head.set_src_server_type(_pserver->GetServerType());
	proxy_head.set_src_server_id(_pserver->GetServerId());

	// send
	LSF_LOG_INF("send response to server, %s, msg_type=%s", LSF_TS(session), LSF_ETS(session.ss_response().msg_type()));
	return ConnectionSendBySequence(session.ss_response());
}

bool ConnectServerMsgTransferService::TransferMessageById(conf::ENServerType server_type, uint32_t server_id, msg::SS& message) {
	// set proxy head
	message.mutable_proxy_head()->set_transfer_type(msg::PROXY_TRANSFER_TYPE_BY_ID);
	message.mutable_proxy_head()->set_dst_server_type(server_type);
	message.mutable_proxy_head()->set_dst_server_id(server_id);
	message.mutable_proxy_head()->set_src_server_type(_pserver->GetServerType());
	message.mutable_proxy_head()->set_src_server_id(_pserver->GetServerId());

	// send
	return ConnectionSendBySequence(message);
}

bool ConnectServerMsgTransferService::TransferMessageByHash(conf::ENServerType server_type, uint64_t hash_id, msg::SS& message) {
	// set proxy head
	message.mutable_proxy_head()->set_transfer_type(msg::PROXY_TRANSFER_TYPE_BY_HASH);
	message.mutable_proxy_head()->set_dst_server_type(server_type);
	message.mutable_proxy_head()->set_dst_hash_id(hash_id);
	message.mutable_proxy_head()->set_src_server_type(_pserver->GetServerType());
	message.mutable_proxy_head()->set_src_server_id(_pserver->GetServerId());

	// send
	return ConnectionSendBySequence(message);
}

bool ConnectServerMsgTransferService::TransferMessageByBroadCast(conf::ENServerType server_type, msg::SS& message) {
	// set proxy head
	message.mutable_proxy_head()->set_transfer_type(msg::PROXY_TRANSFER_TYPE_BY_BROADCAST);
	message.mutable_proxy_head()->set_dst_server_type(server_type);
	message.mutable_proxy_head()->set_src_server_type(_pserver->GetServerType());
	message.mutable_proxy_head()->set_src_server_id(_pserver->GetServerId());

	// send
	return ConnectionSendBySequence(message);
}

bool ConnectServerMsgTransferService::TransferMessageByRandom(conf::ENServerType server_type, msg::SS& message) {
	// set proxy head
	message.mutable_proxy_head()->set_transfer_type(msg::PROXY_TRANSFER_TYPE_BY_RANDOM);
	message.mutable_proxy_head()->set_dst_server_type(server_type);
	message.mutable_proxy_head()->set_src_server_type(_pserver->GetServerType());
	message.mutable_proxy_head()->set_src_server_id(_pserver->GetServerId());

	// send
	return ConnectionSendBySequence(message);
}

bool ConnectServerMsgTransferService::TransferMessageByRandomExceptId(conf::ENServerType server_type, uint32_t except_id, msg::SS& message){
    message.mutable_proxy_head()->set_transfer_type(msg::PROXY_TRANSFER_TYPE_BY_RANDOM_EXCEPT_ID);
    message.mutable_proxy_head()->set_dst_server_type(server_type);
    message.mutable_proxy_head()->set_src_server_type(_pserver->GetServerType());
    message.mutable_proxy_head()->set_src_server_id(_pserver->GetServerId());
    message.mutable_proxy_head()->set_dst_except_id(except_id);

    return ConnectionSendBySequence(message);
}

////////////////////////////////////////////////////////////
// ConnectLogService
bool ConnectLogService::OnInitConfig() {
	// call base
	if (!BasicConnectService::OnInitConfig()) return false;

	// check size
	if (_service_config.connect_addresses_size() != 1) {
		LSF_LOG_ERR("invalid connect address size, %s, %u", LSF_ETS(_service_config.service_type()),
		_service_config.connect_addresses_size());
		return false;
	}

	return true;
}

bool ConnectLogService::SendLogMessage(std::string const& message) {
	return BasicConnectService::ConnectionSendByIndex(0, message);
}

bool ConnectLogService::SendJsonLogFromPb(const ::google::protobuf::Message& message){
    Pb2Json::Json json;
    Pb2Json::Message2Json(message, json);

    string jsonstr = json.dump();
    LSF_LOG_INF("Json log:%s", jsonstr.c_str());
    return BasicConnectService::ConnectionSendByIndex(0, jsonstr);
}

bool ConnectLogService::OnPutSingleMessageIntoStream(std::string  & buffer,std::string const & message){
	buffer.append(message);
	buffer.append("\r\n");
	return true;
}

// vim:ts=4:sw=4:et:ft=cpp:
