// File:        basic_service.cpp
// Description: ---
// Notes:       ---
// Author:      leoxiang <leoxiang727@qq.com>
// Revision:    2015-06-17 by leoxiang

#include "svr/common/basic_service.h"
#include "svr/common/common_header.h"
#include "svr/common/common_proto.h"
#include "svr/common/common_func.h"

using namespace lsf::basic;
using namespace lsf::asio;
using namespace lsf::util;

////////////////////////////////////////////////////////////
// BasicService
////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////
// main routine
bool BasicService::Run(BasicServer* pserver) {
    // check input
    if (pserver == nullptr) {
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

//////////////////////////////////////////////////////////
// async handler
bool BasicService::OnSocketRead(lsf::asio::AsyncInfo& info) {
    std::string message;
    size_t pos = 0;

    while (common::GetSingleMessageFromStream(info.buffer, pos, message)) {
        // callback
        if (!OnConnectionMessage(Socket(info.fd), message)) return false;
    }

    // buffer remain some bytes not used
    if (pos < info.buffer.length()) {
        LSF_LOG_INFO("there is some buffer not used, %s",
                     StringExt::BinToHexString(info.buffer.data() + pos, info.buffer.length() - pos).c_str());
    }

    return true;
}

bool BasicService::OnSocketPeerClose(lsf::asio::AsyncInfo& info) {
    Socket socket(info.fd);
    LSF_LOG_INFO("connection close by peer, local=%s, remote=%s",
            socket.LocalSockAddr().ToCharStr(), socket.RemoteSockAddr().ToCharStr());
    return true;
}

//////////////////////////////////////////////////////////
// default connection handler
bool BasicService::OnConnectionCreate(lsf::asio::Socket socket) { return true; }

bool BasicService::OnConnectionMessage(lsf::asio::Socket socket, std::string& message) {
    LSF_LOG_ERR("you should override this handler");
    return true;
}

bool BasicService::OnConnectionPeerClose(lsf::asio::Socket socket) { return true; }

////////////////////////////////////////////////////////////
// BasicAcceptService
////////////////////////////////////////////////////////////
bool BasicAcceptService::OnInitConfig() {
    for (conf::AcceptService const& conf : _pserver->GetServerConfig().accept_service()) {
        if (conf.service_type() == _service_type) {
            _service_config.CopyFrom(conf);
            return true;
        }
    }

    LSF_LOG_ERR("init config failed, type=%u", _service_type);
    return false;
}

bool BasicAcceptService::OnInitSocket() {
    for (std::string const& iter : _service_config.listen_address()) {
        ListenSocket lsocket = tcp::ListenSocket::CreateListenSocket();
        SockAddr lsockaddr = SockAddr(iter);

        // init listen socket
        if (!lsocket.SetSockReuse()) {
            LSF_LOG_ERR("sockreuse failed, address=%s, %s", lsockaddr.ToCharStr(), lsocket.ErrCharStr());
            lsocket.Close();
            return false;
        }
        if (!lsocket.Bind(lsockaddr)) {
            LSF_LOG_ERR("bind failed, address=%s, %s", lsockaddr.ToCharStr(), lsocket.ErrCharStr());
            lsocket.Close();
            return false;
        }
        if (!lsocket.Listen()) {
            LSF_LOG_ERR("listen failed, address=%s, %s", lsockaddr.ToCharStr(), lsocket.ErrCharStr());
            lsocket.Close();
            return false;
        }

        // async accept
        if (!lsocket.AsyncAccept(*IOService::Instance(), std::bind(&BasicAcceptService::OnSocketAccept, std::ref(*this),
                                                                   std::placeholders::_1))) {
            LSF_LOG_ERR("async accept failed, %s", IOService::Instance()->ErrCharStr());
            lsocket.Close();
            return false;
        }

        LSF_LOG_INFO("init listen socket at %s", lsocket.LocalSockAddr().ToCharStr());
    }

    return true;
}

bool BasicAcceptService::OnSocketAccept(lsf::asio::AsyncInfo& info) {
    // print info
    Socket socket(info.accept_fd);
    LSF_LOG_INFO("accept new connection, local=%s, remote=%s", socket.LocalSockAddr().ToCharStr(),
                 socket.RemoteSockAddr().ToCharStr());

    // async read
    if (!socket.AsyncRead(*IOService::Instance(),
                          std::bind(&BasicAcceptService::OnSocketRead, std::ref(*this), std::placeholders::_1),
                          std::bind(&BasicAcceptService::OnSocketPeerClose, std::ref(*this), std::placeholders::_1))) {
        LSF_LOG_ERR("async read failed, %s", IOService::Instance()->ErrCharStr());
        return false;
    }

    // init sock info
    msg::TcpHead & tcp_head = _acct_sock[socket];
    tcp_head.set_index(socket.GetSockFd());
    tcp_head.set_client_ip(socket.RemoteSockAddr().GetAddress().ToString());
    tcp_head.set_client_port(socket.RemoteSockAddr().GetPort());
    tcp_head.set_is_new_connection(true);
    tcp_head.set_is_close_connection(false);
    tcp_head.set_connect_time(IOService::Instance()->GetClockTime());

    // callback
    return OnConnectionCreate(Socket(info.accept_fd));
}

bool BasicAcceptService::SendMessage(std::string const & buffer, lsf::asio::Socket socket)
{
    // socket.Send(buffer)
    return true;
}

bool BasicAcceptService::BroadcastMessage(std::string const & buffer)
{
    return true;
}

////////////////////////////////////////////////////////////
// BasicConnectService
////////////////////////////////////////////////////////////
bool BasicConnectService::OnInitConfig() {
    // get config
    for (conf::ConnectService const& conf : _pserver->GetServerConfig().connect_service()) {
        if (conf.service_type() == _service_type) {
            // copy config
            _service_config.CopyFrom(conf);

            return true;
        }
    }

    LSF_LOG_ERR("init config failed, type=%u", _service_type);
    return false;
}

bool BasicConnectService::OnInitSocket() {
    // pre-create connection instance
    _conn_scok.insert(_conn_scok.begin(), _service_config.connect_address_size(), Socket(-1));

    // try connect
    for (int i = 0; i < _service_config.connect_address_size(); ++i) {
        if (!TryConnect(i)) return false;
    }

    return true;
}

bool BasicConnectService::OnSocketConnect(lsf::asio::AsyncInfo& info, size_t index) {
    // print info
    Socket socket(info.fd);
    LSF_LOG_INFO("make new connection, local=%s, remote=%s", socket.LocalSockAddr().ToCharStr(),
                 socket.RemoteSockAddr().ToCharStr());

    // async read
    if (!socket.AsyncRead(*IOService::Instance(),
                          std::bind(&BasicService::OnSocketRead, std::ref(*this), std::placeholders::_1),
                          std::bind(&BasicService::OnSocketPeerClose, std::ref(*this), std::placeholders::_1))) {
        LSF_LOG_ERR("async read failed, %s", IOService::Instance()->ErrCharStr());
        return false;
    }

    // asign to socket vector
    _conn_scok[index] = socket;

    // callback
    return OnConnectionCreate(Socket(info.fd));
}

bool BasicConnectService::OnSocketPeerClose(lsf::asio::AsyncInfo& info) {
    // find connect_address
    size_t index;
    for (index = 0; index < _conn_scok.size(); ++index) {
        if (_conn_scok[index] == Socket(info.fd)) break;
    }

    // not found
    if (index == _conn_scok.size()) {
        LSF_LOG_ERR("not found index when reconnn, socket=%d", info.fd);
        return false;
    }

    // reset connection
    _conn_scok[index] = Socket(-1);

    // try reconnect
    if (!TryConnect(index)) return false;

    return true;
}

bool BasicConnectService::TryConnect(size_t index) {
    Socket socket = tcp::Socket::CreateSocket();
    SockAddr sockaddr = SockAddr(_service_config.connect_address(index));

    // sync connect
    if (!socket.Connect(sockaddr)) {
        LSF_LOG_ERR("init connect socket failed, %s", socket.ErrCharStr());
        socket.Close();
        return false;
    }

    // callback
    AsyncInfo info;
    info.fd = socket.GetSockFd();
    if (!OnSocketConnect(info, index)) {
        socket.Close();
        return false;
    }

    return true;
}

bool BasicConnectService::TryAsyncConnect(size_t index) {
    Socket socket = tcp::Socket::CreateSocket();
    SockAddr sockaddr = SockAddr(_service_config.connect_address(index));

    // async connect
    if (!socket.AsyncConnect(
             *IOService::Instance(), sockaddr,
             std::bind(&BasicConnectService::OnSocketConnect, std::ref(*this), std::placeholders::_1, index))) {
        LSF_LOG_ERR("init connect socket failed, %s", IOService::Instance()->ErrCharStr());
        socket.Close();
        return false;
    }

    return true;
}

// vim:ts=4:sw=4:et:ft=cpp:
