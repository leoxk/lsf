// File:        accept_config_service.h
// Description: ---
// Notes:       ---
// Author:      leoxiang <leoxiang727@qq.com>
// Revision:    2015-06-15 by leoxiang

#include "lsf/basic/singleton.hpp"
#include "svr/common/common_proto.h"
#include "svr/common/basic_service.h"

////////////////////////////////////////////////////////////
// AcceptConfigService
class AcceptConfigService : public BasicAcceptService, public lsf::basic::Singleton<AcceptConfigService> {
public:
    AcceptConfigService() : BasicAcceptService(conf::SERVICE_TYPE_CONFIG_CENTER) {}
    void HandleGetServerConfig(lsf::asio::SharedSocket socket, msg::SS const & request);
    void HandleGetAllServerConfig(lsf::asio::SharedSocket socket, msg::SS const & request);
    void HandleGetGameConfig(lsf::asio::SharedSocket socket, msg::SS const & request);

    void ReleaseAllConnections();
protected:
    virtual void OnConnectionMessage(lsf::asio::SharedSocket socket, std::string& message);
    virtual void OnConnectionCreate(lsf::asio::SharedSocket socket);

private:
    using sock_map_type = std::unordered_map<int, lsf::asio::SharedSocket>;
    sock_map_type _sock_map;
};

// vim:ts=4:sw=4:et:ft=cpp:
