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

    while (OnGetSingleMessageFromStream(info.buffer, pos, message)) {
        // callback
        if (!OnConnectionMessage(info.socket, message)) return false;
    }

    // buffer remain some bytes not used
    if (pos < info.buffer.length()) {
        LSF_LOG_INFO("there is some buffer not used, %s",
                     StringExt::BinToHexString(info.buffer.data() + pos, info.buffer.length() - pos).c_str());
    }

    return true;
}

bool BasicService::OnSocketPeerClose(lsf::asio::AsyncInfo& info) {
    // print info
    LSF_LOG_INFO("connection close by peer, local=%s, remote=%s",
            info.socket.LocalSockAddr().ToCharStr(), info.socket.RemoteSockAddr().ToCharStr());

    // callback
    OnSocketClose(info.socket);

    return true;
}

void BasicService::OnSocketClose(lsf::asio::Socket socket) {
    // callback
    OnConnectionClose(socket);
}

bool BasicService::OnGetSingleMessageFromStream(std::string const& buffer, size_t& pos, std::string& message) {
    return common::GetSingleMessageFromStream(buffer, pos, message);
}

bool BasicService::OnPutSingleMessageIntoStream(std::string & buffer, std::string const& message) {
    return common::PutSingleMessageIntoStream(buffer, message);
}

void BasicService::ConnectionClose(lsf::asio::Socket socket) {
    // check input
    if (socket.GetSockFd() < 0) return;

    // must call CloseAsync first
    socket.CloseAsync(IOService::Reference());
    socket.Close();

    // callback
    OnSocketClose(socket);
}

bool BasicService::ConnectionSend(lsf::asio::Socket socket, std::string const &message) {
    // puck into stream
    std::string buffer;
    if (!OnPutSingleMessageIntoStream(buffer, message)) return false;

    // sync send
    if (!socket.Send(buffer, DEF_SEND_TIMEOUT)) {
        LSF_LOG_ERR("send message failed, length=%u, from=%s, to=%s", message.length(),
                socket.LocalSockAddr().ToCharStr(), socket.LocalSockAddr().ToCharStr());
        return false;
    }
    return true;
}

bool BasicService::ConnectionSend(lsf::asio::Socket socket, google::protobuf::MessageLite const &proto_msg) {
    // pack proto
    std::string message;
    if (!common::PackProtoMsg(message, proto_msg)) return false;

    // send
    return ConnectionSend(socket, message);
}

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
    LSF_LOG_INFO("accept new connection, local=%s, remote=%s", info.socket.LocalSockAddr().ToCharStr(),
                 info.socket.RemoteSockAddr().ToCharStr());

    // async read
    if (!info.socket.AsyncRead(*IOService::Instance(),
                          std::bind(&BasicAcceptService::OnSocketRead, std::ref(*this), std::placeholders::_1),
                          std::bind(&BasicAcceptService::OnSocketPeerClose, std::ref(*this), std::placeholders::_1))) {
        LSF_LOG_ERR("async read failed, %s", IOService::Instance()->ErrCharStr());
        return false;
    }

    // callback
    return OnConnectionCreate(info.socket);
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
    _conn_scok.insert(_conn_scok.begin(), _service_config.connect_address_size(), Socket());

    // try connect
    for (int i = 0; i < _service_config.connect_address_size(); ++i) {
        // create socket
        Socket socket = tcp::Socket::CreateSocket();
        SockAddr sockaddr = SockAddr(_service_config.connect_address(i));

        // sync connect
        if (!socket.Connect(sockaddr, DEF_CONNECT_TIMEOUT)) {
            LSF_LOG_ERR("init connect socket failed, sockaddr=%s, %s", sockaddr.ToCharStr(), socket.ErrCharStr());
            ConnectionClose(socket);
            return false;
        }

        // callback
        AsyncInfo info;
        info.socket = socket;
        if (!OnSocketConnect(info, i)) {
            ConnectionClose(socket);
            return false;
        }
    }

    // add connection routine check
    if (!IOService::Instance()->AsyncAddTimerForever(DEF_CONN_CHECK_INTERVAL,
                std::bind(&BasicConnectService::OnConnectionCheck, this))) {
        LSF_LOG_ERR("async add timer failed, %s", IOService::Instance()->ErrCharStr());
        return false;
    }

    return true;
}

bool BasicConnectService::OnSocketConnect(lsf::asio::AsyncInfo& info, size_t index) {
    // check ret
    int ret = info.socket.GetSockError();
    if (ret != 0) {
        LSF_LOG_INFO("connect failed, sockaddr=%s, %s", _service_config.connect_address(index).c_str(), std::strerror(ret));
        return false;
    }

    // print info
    LSF_LOG_INFO("make new connection, local=%s, remote=%s", info.socket.LocalSockAddr().ToCharStr(),
                 info.socket.RemoteSockAddr().ToCharStr());

    // async read
    if (!info.socket.AsyncRead(*IOService::Instance(),
                          std::bind(&BasicService::OnSocketRead, std::ref(*this), std::placeholders::_1),
                          std::bind(&BasicService::OnSocketPeerClose, std::ref(*this), std::placeholders::_1))) {
        LSF_LOG_ERR("async read failed, fd=%u, %s", info.socket.GetSockFd(), IOService::Instance()->ErrCharStr());
        return false;
    }

    // if already has a connection, close first
    if (_conn_scok[index].operator!()) {
        LSF_LOG_FATAL("connect when there is already a connection, sockaddr=%s",
                _service_config.connect_address(index).c_str());
        ConnectionClose(_conn_scok[index]);
    }
    _conn_scok[index] = info.socket;

    // callback
    return OnConnectionCreate(info.socket);
}

bool BasicConnectService::OnConnectionCheck() {
    // travers all connection and check
    for (int i = 0; i < _service_config.connect_address_size(); ++i) {
        // check status
        if (_conn_scok[i].operator!()) continue;

        // create socket
        Socket socket = tcp::Socket::CreateSocket();
        SockAddr sockaddr = SockAddr(_service_config.connect_address(i));

        // async connect
        if (!socket.AsyncConnect(
                 *IOService::Instance(), sockaddr,
                 std::bind(&BasicConnectService::OnSocketConnect, std::ref(*this), std::placeholders::_1, i))) {
            LSF_LOG_ERR("init connect socket failed, %s", IOService::Instance()->ErrCharStr());
            socket.Close();
            return false;
        }
    }

    return true;
}

void BasicConnectService::OnSocketClose(lsf::asio::Socket socket) {
    // erase entry
    for (auto & sock : _conn_scok) {
        if (sock == socket) {
            sock.SetSockFd(-1);
            break;
        }
    }
}

// vim:ts=4:sw=4:et:ft=cpp:
