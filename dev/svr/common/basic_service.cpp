// File:        basic_service.cpp
// Description: ---
// Notes:       ---
// Author:      leoxiang <leoxiang727@qq.com>
// Revision:    2015-06-17 by leoxiang

#include "svr/common/basic_service.h"
#include "svr/common/common_header.h"
#include "svr/common/common_func.h"
#include "svr/common/config_manager.h"

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
        LSF_LOG_ERR("server is null, %s", LSF_ETS(_service_type));
        return false;
    }

    // save server handle
    _pserver = pserver;

    // get service config from server
    if (!OnInitConfig()) return false;

    // init connect socket
    if (!OnInitSocket()) return false;

    // init derived logic
    if (!OnRun()) return false;

    return true;
}

//////////////////////////////////////////////////////////
// async handler
void BasicService::OnSocketRead(lsf::asio::SharedSocket socket, std::string const& buffer) {
    std::string message;
    size_t pos = 0;

    // callback
    while (OnGetSingleMessageFromStream(buffer, pos, message)) {
        if (socket && socket->IsConnect()) OnConnectionMessage(socket, message);
    }

    // if buffer has unused data, just save it
    if (pos < buffer.length()) {
        LSF_LOG_INF("there is some buffer not used, %u, %u", pos, buffer.length());
        IOService::Instance()->SaveUnusedData(socket, buffer.data() + pos, buffer.length() - pos);
    }
}

void BasicService::OnSocketPeerClose(lsf::asio::SharedSocket socket) {
    // print info
    LSF_LOG_INF("connection close by peer, local=%s, remote=%s, fd=%u",
            LSF_TS(socket->LocalSockAddr()), LSF_TS(socket->RemoteSockAddr()), socket->SockFd());

    // callback
    OnConnectionPeerClose(socket);
    OnConnectionClose(socket);
}

bool BasicService::OnGetSingleMessageFromStream(std::string const& buffer, size_t& pos, std::string& message) {
    return common::GetSingleMessageFromStream(buffer, pos, message);
}

bool BasicService::OnPutSingleMessageIntoStream(std::string & buffer, std::string const& message) {
    return common::PutSingleMessageIntoStream(buffer, message);
}

void BasicService::ConnectionClose(lsf::asio::SharedSocket socket) {
    // must call AsyncClose first
    IOService::Instance()->AsyncCancel(socket);
    socket->ShutDown();

    // callback
    OnConnectionClose(socket);
}

bool BasicService::ConnectionSend(lsf::asio::SharedSocket socket, std::string const &message) {
    // check socket is connected
    if (!socket->IsConnect()) {
        LSF_LOG_ERR("socket not connected, %s, from=%s, to=%s, %s", LSF_ETS(_service_type),
                LSF_TS(socket->LocalSockAddr()), LSF_TS(socket->RemoteSockAddr()), LSF_ES(*socket));
        return false;
    }

    // puck into stream
    std::string buffer;
    if (!OnPutSingleMessageIntoStream(buffer, message)) return false;

    // sync send
    if (!socket->Send(buffer, DEF_SEND_TIMEOUT)) {
        LSF_LOG_ERR("send message failed, %s, length=%u, from=%s, to=%s, %s", LSF_ETS(_service_type), buffer.length(),
                LSF_TS(socket->LocalSockAddr()), LSF_TS(socket->RemoteSockAddr()), LSF_ES(*socket));
        return false;
    }
    return true;
}

bool BasicService::ConnectionSend(lsf::asio::SharedSocket socket, google::protobuf::Message const &message) {
    // pack proto
    std::string content;
    if (!Protobuf::SerializeProtoMsgToString(content, message)) {
        LSF_LOG_ERR_STACK("serialize failed, %s", LSF_PROTOBUF);
        return false;
    }

    // send
    return ConnectionSend(socket, content);
}

////////////////////////////////////////////////////////////
// BasicAcceptService
////////////////////////////////////////////////////////////
bool BasicAcceptService::OnInitConfig() {
    for (conf::AcceptService const& conf : ConfigManager::Instance()->ServerConfig().accept_services()) {
        if (conf.service_type() == _service_type) {
            _service_config.CopyFrom(conf);
            return true;
        }
    }

    // check address
    if (_service_config.listen_addresses_size() == 0) {
        LSF_LOG_ERR_STACK("accept service with empty listen address");
        return false;
    }

    LSF_LOG_ERR("init config failed, %s", LSF_ETS(_service_type));
    return false;
}

bool BasicAcceptService::OnInitSocket() {
    for (std::string const& iter : _service_config.listen_addresses()) {
        SockAddr lsockaddr = SockAddr(iter);

        // create socket
        SharedSocket lsocket(proto::tcp);
        if (!lsocket) {
            LSF_LOG_ERR("create socket error, %s", LSF_ES(*lsocket));
            return false;
        }

        // init listen socket
        if (!lsocket->SetSockReuse()) {
            LSF_LOG_ERR("sockreuse failed, %s, address=%s, %s", LSF_ETS(_service_type), LSF_TS(lsockaddr), LSF_ES(*lsocket));
            return false;
        }
        if (!lsocket->Bind(lsockaddr)) {
            LSF_LOG_ERR("bind failed, %s, address=%s, %s", LSF_ETS(_service_type), LSF_TS(lsockaddr), LSF_ES(*lsocket));
            return false;
        }
        if (!lsocket->Listen()) {
            LSF_LOG_ERR("listen failed, %s, address=%s, %s", LSF_ETS(_service_type), LSF_TS(lsockaddr), LSF_ES(*lsocket));
            return false;
        }

        // async accept
        if (!IOService::Instance()->AsyncAccept(lsocket, std::bind(&BasicAcceptService::OnSocketAccept, this,
                        std::placeholders::_1, std::placeholders::_2))) {
            LSF_LOG_ERR("async accept failed, %s, %s", LSF_ETS(_service_type), LSF_ES(*IOService::Instance()));
            return false;
        }

        LSF_LOG_INF("init listen socket %s at %s", LSF_ETS(_service_type), LSF_TS(lsocket->LocalSockAddr()));
    }

    return true;
}

void BasicAcceptService::OnSocketAccept(lsf::asio::SharedSocket listen_socket, lsf::asio::SharedSocket socket) {
    // set buffer length
    socket->SetSendBuf(DEF_SEND_BUF_LEN);
    socket->SetRecvBuf(DEF_RECV_BUF_LEN);

    // print info
    LSF_LOG_INF("accept new connection, %s, local=%s, remote=%s, fd=%d, recvbuf=%u", LSF_ETS(_service_type),
            LSF_TS(socket->LocalSockAddr()), LSF_TS(socket->RemoteSockAddr()), socket->SockFd(), socket->GetRecvBufLen());

    // async read
    if (!IOService::Instance()->AsyncRead(socket,
                          std::bind(&BasicAcceptService::OnSocketRead, this, std::placeholders::_1, std::placeholders::_2),
                          std::bind(&BasicAcceptService::OnSocketPeerClose, this, std::placeholders::_1))) {
        LSF_LOG_ERR("async read failed, %s, %s", LSF_ETS(_service_type), LSF_ES(*IOService::Instance()));
        ConnectionClose(socket);
        return;
    }

    // callback
    OnConnectionCreate(socket);
}

////////////////////////////////////////////////////////////
// BasicConnectService
////////////////////////////////////////////////////////////
bool BasicConnectService::OnInitConfig() {
    // get config
    for (conf::ConnectService const& conf : ConfigManager::Instance()->ServerConfig().connect_services()) {
        if (conf.service_type() == _service_type) {
            // copy config
            _service_config.CopyFrom(conf);

            return true;
        }
    }

    // check address
    if (_service_config.connect_addresses_size() == 0) {
        LSF_LOG_ERR_STACK("connect service with empty connect address");
        return false;
    }

    LSF_LOG_ERR("init config failed, %s", LSF_ETS(_service_type));
    return false;
}

bool BasicConnectService::OnInitSocket() {
    // try connect
    for (int i = 0; i < _service_config.connect_addresses_size(); ++i) {
        // create socket
        _conn_scok.push_back(SharedSocket(proto::tcp));
        SharedSocket socket = _conn_scok.back();
        SockAddr sockaddr = SockAddr(_service_config.connect_addresses(i));

        // sync connect
        if (!socket->Connect(sockaddr, DEF_CONNECT_TIMEOUT)) {
            LSF_LOG_ERR("init connect socket failed, %s, sockaddr=%s, %s", LSF_ETS(_service_type), LSF_TS(sockaddr), LSF_ES(*socket));
            OnSocketConnectFail(socket, sockaddr);
            ConnectionClose(socket);
            return false;
        }

        // callback
        OnSocketConnect(socket, sockaddr, i);

        // if connection close, means something wrong
        if (!socket->IsConnect()) {
            LSF_LOG_ERR_STACK("connection close when init, %s, sockaddr=%s, %s", LSF_ETS(_service_type), LSF_TS(sockaddr), LSF_ES(*socket));
            return false;
        }
    }

    // register connection check callback
    IOService::Instance()->AsyncAddTimerForever(DEF_CONN_CHECK_INTERVAL,
            std::bind(&BasicConnectService::OnConnectionCheck, this));

    return true;
}

void BasicConnectService::OnSocketConnect(lsf::asio::SharedSocket socket, lsf::asio::SockAddr const& sockaddr, size_t index) {
    // print info
    LSF_LOG_INF("make new connection, %s, local=%s, remote=%s, fd=%d, index=%u", LSF_ETS(_service_type),
            LSF_TS(socket->LocalSockAddr()), LSF_TS(socket->RemoteSockAddr()), socket->SockFd(), index);

    // set buffer length
    socket->SetSendBuf(DEF_SEND_BUF_LEN);
    socket->SetRecvBuf(DEF_RECV_BUF_LEN);

    // async read
    if (!IOService::Instance()->AsyncRead(socket,
                          std::bind(&BasicService::OnSocketRead, this, std::placeholders::_1, std::placeholders::_2),
                          std::bind(&BasicService::OnSocketPeerClose, this, std::placeholders::_1))) {
        LSF_LOG_ERR("async read failed, %s, fd=%u, %s", LSF_ETS(_service_type), socket->SockFd(), LSF_ES(*IOService::Instance()));
        ConnectionClose(socket);
        return;
    }

    // callback
    return OnConnectionCreate(socket);
}

void BasicConnectService::OnSocketConnectFail(lsf::asio::SharedSocket socket, lsf::asio::SockAddr const& sockaddr) {
    // print info
    LSF_LOG_ERR("connect failed, %s, sockaddr=%s, %s", LSF_ETS(_service_type), LSF_TS(sockaddr), LSF_ES(*IOService::Instance()));
}

void BasicConnectService::OnConnectionCheck() {
    // travers all connection and check
    for (int i = 0; i < _service_config.connect_addresses_size(); ++i) {
        // check status
        SharedSocket& socket = _conn_scok[i];
        if (socket && socket->IsConnect()) continue;

        // create socket
        socket.reset(proto::tcp);
        SockAddr sockaddr = SockAddr(_service_config.connect_addresses(i));

        // async connect
        if (!IOService::Instance()->AsyncConnect(socket, sockaddr,
                 std::bind(&BasicConnectService::OnSocketConnect, this, std::placeholders::_1, std::placeholders::_2, i),
                 std::bind(&BasicConnectService::OnSocketConnectFail, this, std::placeholders::_1, std::placeholders::_2))) {
            LSF_LOG_ERR("init connect socket failed, sockfd=%u, %s, %s", socket->SockFd(), LSF_ETS(_service_type), LSF_ES(*IOService::Instance()));
        }
    }
}

bool BasicConnectService::ConnectionSendBySequence(google::protobuf::Message const& message) {
    // pack proto
    std::string content;
    if (!Protobuf::SerializeProtoMsgToString(content, message)) {
        LSF_LOG_ERR_STACK("serialize failed, %s", LSF_PROTOBUF);
        return false;
    }

    return ConnectionSendBySequence(content);
}

bool BasicConnectService::ConnectionSendBySequence(std::string const& message) {
    // !!!NOTICE:
    // if consecutive messages within DEF_KEEP_SAME_INDEX_INTERVAL time,
    // then make sure they go through same proxysvr
    if (IOService::Instance()->ClockTimeMilli() > _last_send_time + DEF_KEEP_SAME_INDEX_INTERVAL) {
        _last_random_send_index++;
    }
    _last_send_time = IOService::Instance()->ClockTimeMilli();


    for (size_t i = 0; i < _conn_scok.size(); i++) {
        size_t index = (_last_random_send_index + i) % _conn_scok.size();
        if (_conn_scok[index] && _conn_scok[index]->IsConnect()) {
            return BasicService::ConnectionSend(_conn_scok[index], message);
        }
    }

    LSF_LOG_ERR("all connection is not available, %s", LSF_ETS(_service_type));
    return false;
}

bool BasicConnectService::ConnectionSendByIndex(size_t index, google::protobuf::Message const& message) {
    // pack proto
    std::string content;
    if (!Protobuf::SerializeProtoMsgToString(content, message)) {
        LSF_LOG_ERR_STACK("serialize failed, %s", LSF_PROTOBUF);
        return false;
    }

    return ConnectionSendByIndex(index, content);
}

bool BasicConnectService::ConnectionSendByIndex(size_t index, std::string const& message) {
    if (index >= _conn_scok.size()) {
        LSF_LOG_ERR("invalid socket index, %s, index=%u", LSF_ETS(_service_type), index);
        return false;
    }
    if (!_conn_scok[index]) {
        LSF_LOG_ERR("invalid socket index, %s, index=%u", LSF_ETS(_service_type), index);
        return false;
    }
    if (!_conn_scok[index]->IsConnect()) {
        LSF_LOG_ERR("socket not connected, %s, index=%u", LSF_ETS(_service_type), index);
        return false;
    }
    return ConnectionSend(_conn_scok[index], message);
}

// vim:ts=4:sw=4:et:ft=cpp:
