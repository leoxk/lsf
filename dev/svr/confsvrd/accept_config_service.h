// File:        accept_config_service.h
// Description: ---
// Notes:       ---
// Author:      leoxiang <leoxiang727@qq.com>
// Revision:    2015-06-15 by leoxiang

#include "lsf/basic/singleton.hpp"
#include "svr/common/basic_service.h"

class AcceptConfigService : 
    public BasicAcceptService,
    public lsf::basic::Singleton<AcceptConfigService>
{
public:
    AcceptConfigService() : BasicAcceptService(conf::SERVICE_TYPE_CONFIG_CENTER) { }

protected:
    virtual bool OnConnectionMessage(lsf::asio::Socket socket, std::string & message);
};

// vim:ts=4:sw=4:et:ft=cpp:
