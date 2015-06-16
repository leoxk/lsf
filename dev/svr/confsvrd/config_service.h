// File:        config_service.h
// Description: ---
// Notes:       ---
// Author:      leoxiang <leoxiang727@qq.com>
// Revision:    2015-06-15 by leoxiang

#include "lsf/basic/singleton.hpp"
#include "svr/common/basic_accept_service.h"
#include "svr/common/session_mng.h"

class ConfigService : 
    public BasicAcceptService,
    public lsf::basic::Singleton<ConfigService>
{
public:
    ConfigService() : BasicAcceptService(conf::SERVICE_TYPE_CONFIG_CENTER) { }

public:
    virtual bool OnSocketRead(lsf::asio::AsyncInfo & info);
};

// vim:ts=4:sw=4:et:ft=cpp:
