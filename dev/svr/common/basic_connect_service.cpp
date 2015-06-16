// File:        basic_connect_service.cpp
// Description: ---
// Notes:       ---
// Author:      leoxiang <leoxiang727@qq.com>
// Revision:    2015-06-11 by leoxiang

#include "svr/common/basic_connect_service.h"
#include "svr/common/common_header.h"

using namespace lsf::basic;
using namespace lsf::asio;
using namespace lsf::util;

////////////////////////////////////////////////////////////
// init logic
bool BasicConnectService::Run(BasicServer * pserver)
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

    // init connect socket
    if (!OnInitSocket()) return false;

    return true;
}

bool BasicConnectService::OnInitConfig()
{
    // get config
    for (conf::ConnectService const & conf : _pserver->_server_config.connect_service())
    {
        if (conf.service_type() == _service_type)
        {
            _service_config.CopyFrom(conf);
            return true;
        }
    }

    // pre-create conn instance
    _conn_vec.insert(_conn_vec.begin(), _service_config.connect_address_size(), Socket(-1));
    

    LSF_LOG_ERR("init config failed, type=%u", _service_type);
    return false;
}

bool BasicConnectService::OnInitSocket()
{
    for (int i = 0; i < _service_config.connect_address_size(); ++i)
    {
        Socket socket = tcp::Socket::CreateSocket();
        SockAddr sockaddr = SockAddr(_service_config.connect_address(i));

        // async connect
        if (!socket.AsyncConnect(*IOService::Instance(), sockaddr, 
                    std::bind(&BasicConnectService::OnSocketConnect, std::ref(*this), std::placeholders::_1, i)))
        {
            LSF_LOG_ERR("init connect socket failed, %s", IOService::Instance()->ErrCharStr());
            socket.Close();
            return false;
        }
    }

    return true;
}

//////////////////////////////////////////////////////////
// async handler
bool BasicConnectService::OnSocketConnect(lsf::asio::AsyncInfo & info, int index)
{
    // print info
    Socket socket(info.fd);
    LSF_LOG_INFO("make new connection, local=%s, remote=%s", 
            socket.LocalSockAddr().ToCharStr(), 
            socket.RemoteSockAddr().ToCharStr());

    // async read
    if (!socket.AsyncRead(*IOService::Instance(), 
                std::bind(&BasicConnectService::OnSocketRead, std::ref(*this), std::placeholders::_1), 
                std::bind(&BasicConnectService::OnSocketPeerClose, std::ref(*this), std::placeholders::_1)))
    {
        LSF_LOG_ERR("async read failed, %s", IOService::Instance()->ErrCharStr());
        return false;
    }

    // asign to socket vector
    _conn_vec[index] = socket;

    return true;
}

bool BasicConnectService::OnSocketRead(lsf::asio::AsyncInfo & info)
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

bool BasicConnectService::OnSocketPeerClose(lsf::asio::AsyncInfo & info)
{
    return true;
}

//////////////////////////////////////////////////////////
// default connection handler
bool BasicConnectService::OnConnectionCreate(lsf::asio::Socket socket, lsf::asio::Socket listen_socket)
{
    return true;
}

bool BasicConnectService::OnConnectionMessage(lsf::asio::Socket socket, std::string & buffer)
{
    LSF_LOG_ERR("you should override this handler");
    return true;
}

bool BasicConnectService::OnConnectionPeerClose(lsf::asio::Socket socket)
{
    return true;
}

// vim:ts=4:sw=4:et:ft=cpp:
