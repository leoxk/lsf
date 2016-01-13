// File:        data_server.h
// Description: ---
// Notes:       ---
// Author:      leoxiang <leoxiang727@qq.com>
// Revision:    2015-08-06 by leoxiang

#include "lsf/basic/singleton.hpp"
#include "svr/common/basic_server.h"

////////////////////////////////////////////////////////////
// DataServer
class DataServer : public BasicServer, public lsf::basic::Singleton<DataServer> {
public:
    DataServer() : BasicServer(conf::SERVER_TYPE_DATA_SERVER) {}

public:
    virtual bool OnRun();
};

// vim:ts=4:sw=4:et:ft=cpp:
