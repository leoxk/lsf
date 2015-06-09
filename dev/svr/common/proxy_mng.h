// File:        proxy_mng.h
// Description: ---
// Notes:       ---
// Author:      leoxiang <leoxiang727@qq.com>
// Revision:    2015-06-09 by leoxiang

#include <vector>
#include "lsf/basic/singleton.hpp"
#include "lsf/asio/tcp.hpp"
#include "svr/proto/conf_deploy.pb.h"

class ProxyManager: public lsf::basic::Singleton<ProxyManager>
{
public:
    bool Init(conf::Server const & server_config);

private:
    std::vector<lsf::asio::tcp::Socket *> conn_vec;
};

// vim:ts=4:sw=4:et:ft=cpp:
