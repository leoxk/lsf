// File:        common_service.h
// Description: ---
// Notes:       ---
// Author:      leoxiang <leoxiang727@qq.com>
// Revision:    2015-06-17 by leoxiang

#include "lsf/basic/singleton.hpp"
#include "svr/common/basic_service.h"

////////////////////////////////////////////////////////////
// ConnectConfigService
class ConnectConfigService : public BasicConnectService, public lsf::basic::Singleton<ConnectConfigService> {
public:
    ConnectConfigService() : BasicConnectService(conf::SERVICE_TYPE_CONFIG_CENTER) {}

    void SetConfigServerAddress(std::string const& address);

    // do not init config from server config
    // because server config is not init yet
    virtual bool OnInitConfig() { return true; }

protected:
    virtual bool OnConnectionCreate(lsf::asio::Socket socket);
};

////////////////////////////////////////////////////////////
// ConnectClientMsgTransferService
class ConnectClientMsgTransferService : public BasicConnectService, public lsf::basic::Singleton<ConnectClientMsgTransferService> {
public:
    ConnectClientMsgTransferService() : BasicConnectService(conf::SERVICE_TYPE_CLIENT_MSG_TRANSFER) {}

protected:
    virtual bool OnConnectionCreate(lsf::asio::Socket socket);
    virtual bool OnConnectionMessage(lsf::asio::Socket socket, std::string & message);

protected:
    lsf::asio::Socket _socket;
};

// vim:ts=4:sw=4:et:ft=cpp:
