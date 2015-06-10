// File:        proxy_mng.cpp
// Description: ---
// Notes:       ---
// Author:      leoxiang <leoxiang727@qq.com>
// Revision:    2015-06-09 by leoxiang

#include "lsf/util/log.hpp"
#include "svr/common/proxy_mng.h"

using namespace lsf::asio;
using namespace lsf::util;

bool ProxyManager::Init(conf::Server const & server_config)
{
    for (int i = 0; i < server_config.proxy_address_size(); ++i)
    {
        tcp::Socket socket = tcp::Socket::CreateSocket();
        tcp::SockAddr sockaddr(
                server_config.proxy_address(i).ip(), 
                server_config.proxy_address(i).port());

        // connect
        if (!socket.Connect(sockaddr))
        {
            LSF_LOG_ERR("address=%s, %s", sockaddr.ToCharStr(), socket.ErrCharStr());
            continue;
        }

        // TODO
        //// register
        //// send request
        //msg::SS message;
        //std::string content;
        //message.set_type(msg::SS_GET_CONFIG_REQ);
        //message.mutable_get_config_req()->set_server_type(_server_type);
        //message.mutable_get_config_req()->set_server_id(_server_id);
        //if (!message.SerializeToString(&content))
        //{
            //LSF_LOG_ERR("%s", ProtobufLog::Instance()->ErrCharStr());
            //socket.Close();
            //return false;
        //}
        //if (!socket.SendAll(content))
        //{
            //LSF_LOG_ERR("size=%u, %s", content.size(), socket.ErrCharStr());
            //socket.Close();
            //return false;
        //}

        //// get response
        //if (!socket.RecvAll(content))
        //{
            //LSF_LOG_ERR("%s", socket.ErrCharStr());
            //socket.Close();
            //return false;
        //}

        //// parse config
    }
    return true;
}

// vim:ts=4:sw=4:et:ft=cpp:
