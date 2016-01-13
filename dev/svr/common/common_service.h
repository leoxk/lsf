// File:        common_service.h
// Description: ---
// Notes:       ---
// Author:      leoxiang <leoxiang727@qq.com>
// Revision:    2015-06-17 by leoxiang

#pragma once
#include "lsf/basic/singleton.hpp"
#include "svr/common/common_header.h"
#include "svr/common/common_proto.h"
#include "svr/common/basic_service.h"
#include "svr/common/session_manager.h"

////////////////////////////////////////////////////////////
// ConnectConfigService
class ConnectConfigService : public BasicConnectService, public lsf::basic::Singleton<ConnectConfigService> {
public:
    using config_reload_func = std::function<void()>;

public:
    ConnectConfigService() : BasicConnectService(conf::SERVICE_TYPE_CONFIG_CENTER) {}
    void SetConfigServiceAddress(std::string const& address);
    bool GetAllServerConfig(google::protobuf::RepeatedPtrField<conf::Server>& configs);

    template<typename HandlerType>
    void AddGameConfigReloadHandle(HandlerType&& handler) {
        _game_config_reload_func_set.push_back(std::forward<HandlerType>(handler));
    }

    template<typename HandlerType>
    void AddDeployConfigReloadHandle(HandlerType&& handler) {
        _deploy_config_reload_func_set.push_back(std::forward<HandlerType>(handler));
    }

protected:
    bool InitServerConfig();
    bool InitGameConfig();

    // do not init config from server config
    // because server config is not init yet
    virtual bool OnInitConfig() { return true; }
    virtual void OnConnectionCreate(lsf::asio::SharedSocket socket);

private:
    std::vector<config_reload_func> _game_config_reload_func_set;
    std::vector<config_reload_func> _deploy_config_reload_func_set;

};

////////////////////////////////////////////////////////////
// ConnectClientMsgTransferService
class ConnectClientMsgTransferService : public BasicConnectService, public lsf::basic::Singleton<ConnectClientMsgTransferService> {
public:
    ConnectClientMsgTransferService() : BasicConnectService(conf::SERVICE_TYPE_CLIENT_MSG_TRANSFER) {}
    virtual void OnConnectionCreate(lsf::asio::SharedSocket socket);
    virtual void OnConnectionMessage(lsf::asio::SharedSocket socket, std::string& content);

    bool TransferResponse(Session& session);
    bool TransferMessageByConnId(uint32_t conn_id, msg::CS& message);
    bool TransferMessageByConnIds(google::protobuf::RepeatedField<uint32_t> const& conn_ids, msg::CS& message);
    msg::ConnSvrInfo const& GetConnSvrInfo() const { return _conn_svr_info; }

private:
    msg::ConnSvrInfo _conn_svr_info;
};

////////////////////////////////////////////////////////////
// ConnectServerMsgTransferService
class ConnectServerMsgTransferService : public BasicConnectService, public lsf::basic::Singleton<ConnectServerMsgTransferService> {
public:
    ConnectServerMsgTransferService() : BasicConnectService(conf::SERVICE_TYPE_SERVER_MSG_TRANSFER) {}
    virtual bool OnRun();
    virtual void OnConnectionCreate(lsf::asio::SharedSocket socket);
    virtual void OnConnectionMessage(lsf::asio::SharedSocket socket, std::string& content);

    bool TransferResponse(Session& session);
    bool TransferMessageById(conf::ENServerType server_type, uint32_t server_id, msg::SS& message);
    bool TransferMessageByHash(conf::ENServerType server_type, uint64_t hash_id, msg::SS& message);
    bool TransferMessageByBroadCast(conf::ENServerType server_type, msg::SS& message);
    bool TransferMessageByRandom(conf::ENServerType server_type, msg::SS& message);
    bool TransferMessageByRandomExceptId(conf::ENServerType server_type,uint32_t except_id, msg::SS& message);
    void PostInternalMessage(msg::SS& message);
    void PostAsyncInternalMessage(msg::SS& message);
};

/////////////////////////////////////////////////////////////
// ConnectLogService
class ConnectLogService : public BasicConnectService, public lsf::basic::Singleton<ConnectLogService>{
public:
    ConnectLogService(): BasicConnectService(conf::SERVICE_TYPE_NET_LOG){}

    virtual bool OnInitConfig();
    bool SendLogMessage(std::string const& message);
    bool SendJsonLogFromPb(const ::google::protobuf::Message& message); 
    virtual bool OnPutSingleMessageIntoStream(std::string & buffer,std::string const& message);
};


// vim:ts=4:sw=4:et:ft=cpp:
