// File:        basic_service.h
// Description: ---
// Notes:       ---
// Author:      leoxiang <leoxiang727@qq.com>
// Revision:    2015-06-17 by leoxiang

#pragma once
#include <vector>
#include "lsf/asio/tcp.hpp"
#include "lsf/asio/net.hpp"
#include "lsf/basic/noncopyable.hpp"
#include "lsf/basic/error.hpp"
#include "svr/proto/conf_deploy.pb.h"
#include "svr/common/basic_server.h"

////////////////////////////////////////////////////////////
// BasicService
class BasicService : public lsf::basic::NonCopyable {
public:
    static const size_t DEF_SEND_TIMEOUT = 1000; // milli seconds
    friend class lsf::asio::ProactorSerivce;

public:
    BasicService(conf::ENServiceType service_type) : _service_type(service_type) {}
    virtual ~BasicService() { }
    bool Run(BasicServer* pserver);
    // most of time you should not mess with these callbacks
    virtual bool OnSocketRead(lsf::asio::Socket socket, std::string const& buffer);
    virtual void OnSocketPeerClose(lsf::asio::Socket socket);
    virtual void OnSocketClose(lsf::asio::Socket socket);
    virtual bool OnGetSingleMessageFromStream(std::string const& buffer, size_t& pos, std::string& message);
    virtual bool OnPutSingleMessageIntoStream(std::string & buffer, std::string const& message);

protected:
    // init logic
    virtual bool OnInitConfig() = 0;
    virtual bool OnInitSocket() = 0;

    // connection handler
    void ConnectionClose(lsf::asio::Socket socket);
    bool ConnectionSend(lsf::asio::Socket socket, std::string const& message);
    bool ConnectionSend(lsf::asio::Socket socket, google::protobuf::MessageLite const& proto_msg);
    virtual bool OnConnectionCreate(lsf::asio::Socket socket) { return true; }
    virtual bool OnConnectionMessage(lsf::asio::Socket socket, std::string& message) { return true; }
    virtual void OnConnectionPeerClose(lsf::asio::Socket socket) { }
    virtual void OnConnectionClose(lsf::asio::Socket socket) { }

protected:
    conf::ENServiceType _service_type;
    BasicServer* _pserver = nullptr;
};

////////////////////////////////////////////////////////////
// BasicAcceptService
class BasicAcceptService : public BasicService {
public:
    friend class lsf::asio::ProactorSerivce;

public:
    BasicAcceptService(conf::ENServiceType service_type) : BasicService(service_type) {}
    virtual ~BasicAcceptService() {}
    virtual bool OnSocketAccept(lsf::asio::Socket socket, lsf::asio::ListenSocket listen_socket);

protected:
    virtual bool OnInitConfig();
    virtual bool OnInitSocket();

protected:
    conf::AcceptService _service_config;
};

////////////////////////////////////////////////////////////
// BasicService
class BasicConnectService : public BasicService {
public:
    using conn_scok_type = std::vector<lsf::asio::Socket>;
    friend class lsf::asio::ProactorSerivce;

    static const size_t DEF_CONN_CHECK_INTERVAL = 10*1000; // milli seconds
    static const size_t DEF_CONNECT_TIMEOUT = 5*1000; // milli seconds

public:
    BasicConnectService(conf::ENServiceType service_type) : BasicService(service_type) {}
    virtual ~BasicConnectService() {}
    virtual bool OnSocketConnect(lsf::asio::Socket socket, lsf::asio::SockAddr const& sockaddr, size_t index);
    virtual void OnSocketConnectFail(lsf::asio::Socket socket, lsf::asio::SockAddr const& sockaddr);
    virtual void OnSocketClose(lsf::asio::Socket socket);

protected:
    virtual bool OnInitConfig();
    virtual bool OnInitSocket();
    virtual bool OnConnectionCheck();

protected:
    conf::ConnectService _service_config;
    conn_scok_type _conn_scok;
};

// vim:ts=4:sw=4:et:ft=cpp:
