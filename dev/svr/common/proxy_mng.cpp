// File:        proxy_mng.cpp
// Description: ---
// Notes:       ---
// Author:      leoxiang <leoxiang727@qq.com>
// Revision:    2015-06-09 by leoxiang

#include "svr/common/proxy_mng.h"
#include "svr/common/common_func.h"
#include "svr/proto/msg_ss.pb.h"

using namespace lsf::asio;
using namespace lsf::util;

bool ProxyManager::Init(conf::Server const & server_config)
{
    // TODO
    //for (int i = 0; i < server_config.proxy_address_size(); ++i)
    //{
        //tcp::Socket socket = tcp::Socket::CreateSocket();
        //tcp::SockAddr sockaddr(
                //server_config.proxy_address(i).ip(), 
                //server_config.proxy_address(i).port());

        //// connect
        //if (!socket.Connect(sockaddr))
        //{
            //LSF_LOG_ERR("address=%s, %s", sockaddr.ToCharStr(), socket.ErrCharStr());
            //continue;
        //}

        //// contruct message
        //msg::SS message;
        //std::string content;
        //message.set_type(msg::SS_REGISTER_TO_RPOXY_REQ);
        //message.mutable_register_to_proxy_req()->set_server_type(server_config.server_type());
        //message.mutable_register_to_proxy_req()->set_server_id(server_config.server_id());

        //// send and recv
        //if (!CommonFunc::SendAndRecv(socket, message))
        //{
            //// TODO
        //}
    //}
    return true;
}

// vim:ts=4:sw=4:et:ft=cpp:
