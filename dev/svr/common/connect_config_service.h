// File:        connect_config_service.h
// Description: ---
// Notes:       ---
// Author:      leoxiang <leoxiang727@qq.com>
// Revision:    2015-06-17 by leoxiang

#include "lsf/basic/singleton.hpp"
#include "svr/common/basic_service.h"

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

// vim:ts=4:sw=4:et:ft=cpp:
