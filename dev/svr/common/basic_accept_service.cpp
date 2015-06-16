// File:        basic_accept_service.cpp
// Description: ---
// Notes:       ---
// Author:      leoxiang <leoxiang727@qq.com>
// Revision:    2015-06-11 by leoxiang

#include "svr/common/basic_accept_service.h"
#include "svr/common/common_header.h"

using namespace lsf::basic;
using namespace lsf::asio;
using namespace lsf::util;

////////////////////////////////////////////////////////////
// init logic
bool BasicAcceptService::Run(BasicServer * pserver)
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
    if (!OnInitSocket()) return false;

    return true;
}

bool BasicAcceptService::OnInitConfig()
{
    for (conf::AcceptService const & conf : _pserver->_server_config.accept_service())
    {
        if (conf.service_type() == _service_type)
        {
            _service_config.CopyFrom(conf);
            return true;
        }
    }

    LSF_LOG_ERR("init config failed, type=%u", _service_type);
    return false;
}

bool BasicAcceptService::OnInitSocket()
{
    for (std::string const & iter : _service_config.listen_address())
    {
        ListenSocket lsocket = tcp::ListenSocket::CreateListenSocket();
        SockAddr lsockaddr = SockAddr(iter);

        // init listen socket
        if (!lsocket.Bind(lsockaddr))
        {
            LSF_LOG_ERR("bind failed, address=%s, %s", lsockaddr.ToCharStr(), lsocket.ErrCharStr());
            lsocket.Close();
            return false;
        }
        if (!lsocket.Listen())
        {
            LSF_LOG_ERR("listen failed, address=%s, %s", lsockaddr.ToCharStr(), lsocket.ErrCharStr());
            lsocket.Close();
            return false;
        }

        // async accept
        if (!lsocket.AsyncAccept(*IOService::Instance(), std::bind(&BasicAcceptService::OnSocketAccept, std::ref(*this), std::placeholders::_1)))
        {
            LSF_LOG_ERR("async accept failed, %s", IOService::Instance()->ErrCharStr());
            lsocket.Close();
            return false;
        }

        LSF_LOG_INFO("init listen socket at %s", lsocket.LocalSockAddr().ToCharStr());
    }

    return true;
}

//////////////////////////////////////////////////////////
// async handler
bool BasicAcceptService::OnSocketAccept(lsf::asio::AsyncInfo & info)
{
    // print info
    Socket socket(info.accept_fd);
    LSF_LOG_INFO("accept new connection, local=%s, remote=%s", 
            socket.LocalSockAddr().ToCharStr(), 
            socket.RemoteSockAddr().ToCharStr());

    // async read
    if (!socket.AsyncRead(*IOService::Instance(), 
                std::bind(&BasicAcceptService::OnSocketRead, std::ref(*this), std::placeholders::_1), 
                std::bind(&BasicAcceptService::OnSocketPeerClose, std::ref(*this), std::placeholders::_1)))
    {
        LSF_LOG_ERR("async read failed, %s", IOService::Instance()->ErrCharStr());
        return false;
    }

    // callback
    return OnConnectionCreate(Socket(info.accept_fd), Socket(info.fd));
}

bool BasicAcceptService::OnSocketRead(lsf::asio::AsyncInfo & info)
{
    size_t pos = 0;
    char const * begin = info.buffer.data();
    size_t total = info.buffer.size();

    while (pos < total)
    {
        // check length
        if (pos + sizeof(common::MAGIC_STRING) + sizeof(uint32_t) >= total)
        {
            LSF_LOG_ERR("get incomplete message, %s", BinToHexString(begin + pos, total - pos).c_str());
            return true;
        }

        // check magic string
        if (::memcmp(begin + pos, common::MAGIC_STRING, sizeof(common::MAGIC_STRING)) != 0)
        {
            LSF_LOG_ERR("message invalid magic string, %s", BinToHexString(begin + pos, sizeof(common::MAGIC_STRING)).c_str());
            return true;
        }
        pos += sizeof(common::MAGIC_STRING);

        // get size
        uint32_t size = ntohl(*((uint32_t *)(begin + pos)));
        pos += sizeof(uint32_t);

        // check length
        if (pos + size > total)
        {
            LSF_LOG_ERR("get incomplete message, size=%u, left_size=%u, %s", 
                    size, total - pos, BinToHexString(begin + pos, total - pos).c_str());
            return true;
        }

        // get message
        std::string message(info.buffer, pos, size);
        pos += size;

        // callback
        if (!OnConnectionMessage(Socket(info.fd), message)) return false;
    }

    return true;
}

bool BasicAcceptService::OnSocketPeerClose(lsf::asio::AsyncInfo & info)
{
    return true;
}

//////////////////////////////////////////////////////////
// default connection handler
bool BasicAcceptService::OnConnectionCreate(lsf::asio::Socket socket, lsf::asio::Socket listen_socket)
{
    return true;
}

bool BasicAcceptService::OnConnectionMessage(lsf::asio::Socket socket, std::string & buffer)
{
    LSF_LOG_ERR("you should override this handler");
    return true;
}

bool BasicAcceptService::OnConnectionPeerClose(lsf::asio::Socket socket)
{
    return true;
}

// vim:ts=4:sw=4:et:ft=cpp:
