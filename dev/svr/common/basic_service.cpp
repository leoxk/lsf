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
bool BasicService::Run()
{
    // check input
    if (_service_type != _service_config.service_type())
    {
        LSF_LOG_ERR("service_type=%u %u", _service_type, _service_config.service_type());
        return false;
    }

    // init listen socket
    if (!OnInitListenSocket()) return false;

    // init connect socket
    if (!OnInitConnectSocket()) return false;

    return true;
}

bool BasicService::OnInitListenSocket()
{
    for (int i = 0; i < _service_config.listen_address_size(); ++i)
    {
        tcp::ListenSocket lsocket = tcp::ListenSocket::CreateListenSocket();
        tcp::SockAddr lsockaddr = tcp::SockAddr(_service_config.listen_address(i));

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
        if (!lsocket.AsyncAccept(_io_service, std::bind(&BasicService::OnSocketAccept, std::ref(*this), std::placeholders::_1)))
        {
            LSF_LOG_ERR("%s", _io_service.ErrCharStr());
            lsocket.Close();
            return false;
        }

        LSF_LOG_INFO("init listen socket at %s", lsocket.LocalSockAddr().ToCharStr());
    }

    return true;
}

bool BasicService::OnInitConnectSocket()
{
    for (int i = 0; i < _service_config.connect_address_size(); ++i)
    {
        tcp::Socket socket = tcp::Socket::CreateSocket();
        tcp::SockAddr sockaddr = tcp::SockAddr(_service_config.connect_address(i));

        // async connect
        if (!socket.AsyncConnect(_io_service, sockaddr, 
                    std::bind(&BasicService::OnSocketConnect, std::ref(*this), std::placeholders::_1)))
        {
            LSF_LOG_ERR("%s", _io_service.ErrCharStr());
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
    tcp::Socket socket(info.accept_fd);

    LSF_LOG_INFO("accept new connection, local=%s, remote=%s", 
            socket.LocalSockAddr().ToCharStr(), 
            socket.RemoteSockAddr().ToCharStr());

    return OnConnectionCreate(socket);
}

bool BasicService::OnSocketConnect(lsf::asio::AsyncInfo & info)
{
    tcp::Socket socket(info.fd);

    LSF_LOG_INFO("make new connection, local=%s, remote=%s", 
            socket.LocalSockAddr().ToCharStr(), 
            socket.RemoteSockAddr().ToCharStr());

    return OnConnectionCreate(socket);
}

bool BasicService::OnSocketRead(lsf::asio::AsyncInfo & info)
{
    return OnConnectionRead(tcp::Socket(info.fd));
}

bool BasicService::OnSocketPeerClose(lsf::asio::AsyncInfo & info)
{
    return OnConnectionPeerClose(tcp::Socket(info.fd));
}

//////////////////////////////////////////////////////////
// connection callback
bool BasicService::OnConnectionCreate(lsf::asio::tcp::Socket socket)
{
    // async read
    if (!socket.AsyncRead(_io_service, 
                std::bind(&BasicService::OnSocketRead, std::ref(*this), std::placeholders::_1), 
                std::bind(&BasicService::OnSocketPeerClose, std::ref(*this), std::placeholders::_1)))
    {
        LSF_LOG_ERR("%s", _io_service.ErrCharStr());
        return false;
    }

    return true;
}

bool BasicService::OnConnectionRead(lsf::asio::tcp::Socket socket)
{
    // error
    LSF_LOG_ERR("you should override this handler");
    return true;
}

bool BasicService::OnConnectionPeerClose(lsf::asio::tcp::Socket socket)
{
    // error
    LSF_LOG_ERR("you should override this handler");
    return true;
}

// vim:ts=4:sw=4:et:ft=cpp:
