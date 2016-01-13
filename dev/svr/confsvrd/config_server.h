// File:        config_server.h
// Description: ---
// Notes:       ---
// Author:      leoxiang <leoxiang727@qq.com>
// Revision:    2015-06-08 by leoxiang

#include "lsf/basic/singleton.hpp"
#include "svr/common/basic_server.h"

////////////////////////////////////////////////////////////
// ConfigServer
class ConfigServer : public BasicServer, public lsf::basic::Singleton<ConfigServer> {
public:
    ConfigServer() : BasicServer(conf::SERVER_TYPE_CONFIG_CENTER) {}

public:
    virtual bool OnParseCommond();
    virtual bool OnInitConfigService();
    virtual bool OnRun();
};

// vim:ts=4:sw=4:et:ft=cpp:
