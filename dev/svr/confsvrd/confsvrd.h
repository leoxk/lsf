// File:        confsvrd.h
// Description: ---
// Notes:       ---
// Author:      leoxiang <leoxiang727@qq.com>
// Revision:    2015-06-08 by leoxiang

#include "svr/common/basic_server.h"
#include "svr/common/common_func.h"

class ConfigServer : 
    public BasicServer,
    public lsf::basic::Singleton<ConfigServer>
{
public:
    ConfigServer() : BasicServer(conf::SERVER_TYPE_CONFIG_CENTER) { }

public:
    virtual bool OnParseCommond(int argc, char** argv);

    virtual bool OnGetConfig();

    virtual bool OnInitProxy() { return true; }

    virtual bool OnInitNetLog() { return true; }

    virtual bool OnRun();

    virtual bool OnClientMessage();

private:
    std::string _config_path;
};

// vim:ts=4:sw=4:et:ft=cpp:
