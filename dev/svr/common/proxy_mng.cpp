// File:        proxy_mng.cpp
// Description: ---
// Notes:       ---
// Author:      leoxiang <leoxiang727@qq.com>
// Revision:    2015-06-09 by leoxiang

#include "proxy_mng.h"

using namespace lsf::asio;

bool ProxyManager::Init(conf::Server const & server_config)
{
    for (int i = 0; i < server_config.proxysvrd_address_size(); ++i)
    {
        // TODO
        //tcp::Socket socket;
        
    }
    return true;
}

// vim:ts=4:sw=4:et:ft=cpp:
