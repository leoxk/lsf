// File:        basic_service.cpp
// Description: ---
// Notes:       ---
// Author:      leoxiang <leoxiang727@qq.com>
// Revision:    2015-06-11 by leoxiang

#include "svr/common/basic_service.h"
#include "svr/common/common_header.h"

using namespace lsf::basic;
using namespace lsf::asio;
using namespace lsf::util;

////////////////////////////////////////////////////////////
// init logic
bool BasicService::Run(BasicServer * pserver)
{
    // check input
    if (pserver == NULL)
    {
        LSF_LOG_ERR("server is null");
        return false;
    }

    // save server handle
    _pserver = pserver;

    // get service config from server
    if (!OnInitConfig()) return false;

    // init listen socket
    if (!OnInitListenSocket()) return false;

    // init connect socket
    if (!OnInitConnectSocket()) return false;

    return true;
}

bool BasicService::OnInitConfig()
{
    for (conf::Service const & conf : _pserver->_server_config.services())
    {
        if (conf.service_type() == _service_type)
        {
            _service_config.CopyFrom(conf);
            return true;
        }
    }

    LSF_LOG_ERR("type=%u", _service_type);
    return false;
}

bool BasicService::OnInitListenSocket()
{
    for (auto iter : _service_config.listen_address())
    {
        tcp::ListenSocket lsocket = tcp::ListenSocket::CreateListenSocket();
        tcp::SockAddr lsockaddr = tcp::SockAddr(iter);

        // init listen socket
        if (!lsocket.Bind(lsockaddr))
        {
            LSF_LOG_ERR("address=%s, %s", lsockaddr.ToCharStr(), lsocket.ErrCharStr());
            lsocket.Close();
            return false;
        }
        if (!lsocket.Listen())
        {
            LSF_LOG_ERR("address=%s, %s", lsockaddr.ToCharStr(), lsocket.ErrCharStr());
            lsocket.Close();
            return false;
        }

        // async accept
        if (!lsocket.AsyncAccept(*IOService::Instance(), std::bind(&BasicService::OnSocketAccept, std::ref(*this), std::placeholders::_1)))
        {
            LSF_LOG_ERR("%s", IOService::Instance()->ErrCharStr());
            lsocket.Close();
            return false;
        }

        LSF_LOG_INFO("init listen socket at %s", lsocket.LocalSockAddr().ToCharStr());
    }

    return true;
}

bool BasicService::OnInitConnectSocket()
{
    for (auto iter : _service_config.connect_address())
    {
        tcp::Socket socket = tcp::Socket::CreateSocket();
        tcp::SockAddr sockaddr = tcp::SockAddr(iter);

        // async connect
        if (!socket.AsyncConnect(*IOService::Instance(), sockaddr, 
                    std::bind(&BasicService::OnSocketConnect, std::ref(*this), std::placeholders::_1)))
        {
            LSF_LOG_ERR("%s", IOService::Instance()->ErrCharStr());
            socket.Close();
            return false;
        }
    }

    return true;
}

//////////////////////////////////////////////////////////
// async handler
bool BasicService::OnSocketAccept(lsf::asio::AsyncInfo & info)
{
    // print info
    tcp::Socket socket(info.accept_fd);
    LSF_LOG_INFO("accept new connection, local=%s, remote=%s", 
            socket.LocalSockAddr().ToCharStr(), 
            socket.RemoteSockAddr().ToCharStr());

    // async read
    if (!socket.AsyncRead(*IOService::Instance(), 
                std::bind(&BasicService::OnSocketRead, std::ref(*this), std::placeholders::_1), 
                std::bind(&BasicService::OnSocketPeerClose, std::ref(*this), std::placeholders::_1)))
    {
        LSF_LOG_ERR("%s", IOService::Instance()->ErrCharStr());
        return false;
    }

    return true;
}

bool BasicService::OnSocketConnect(lsf::asio::AsyncInfo & info)
{
    // print info
    tcp::Socket socket(info.fd);
    LSF_LOG_INFO("make new connection, local=%s, remote=%s", 
            socket.LocalSockAddr().ToCharStr(), 
            socket.RemoteSockAddr().ToCharStr());

    // async read
    if (!socket.AsyncRead(*IOService::Instance(), 
                std::bind(&BasicService::OnSocketRead, std::ref(*this), std::placeholders::_1), 
                std::bind(&BasicService::OnSocketPeerClose, std::ref(*this), std::placeholders::_1)))
    {
        LSF_LOG_ERR("%s", IOService::Instance()->ErrCharStr());
        return false;
    }

    return true;
}

bool BasicService::OnSocketRead(lsf::asio::AsyncInfo & info)
{
    LSF_LOG_ERR("you should override this handler");
    return true;
}

bool BasicService::OnSocketPeerClose(lsf::asio::AsyncInfo & info)
{
    return true;
}

// vim:ts=4:sw=4:et:ft=cpp:
