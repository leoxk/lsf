// File:        basic_service.h
// Description: ---
// Notes:       ---
// Author:      leoxiang <leoxiang727@qq.com>
// Revision:    2015-06-17 by leoxiang

#pragma once
#include <vector>
#include "svr/common/common_header.h"
#include "svr/common/common_proto.h"
#include "svr/common/basic_server.h"

////////////////////////////////////////////////////////////
// BasicService
class BasicConnectService;
class BasicAcceptService;
class BasicService : public lsf::basic::NonCopyable {
public:
    static const size_t DEF_SEND_TIMEOUT = 3000; // milli seconds
    static const size_t DEF_SEND_BUF_LEN = 1024*1024;
    static const size_t DEF_RECV_BUF_LEN = 1024*1024;
    friend class lsf::asio::ProactorSerivce;
    friend class BasicConnectService;
    friend class BasicAcceptService;

public:
    BasicService(conf::ENServiceType service_type) : _service_type(service_type) {}
    virtual ~BasicService() { }
    bool Run(BasicServer* pserver);

protected:
    // most of time you should not mess with these funcs
    virtual void OnSocketRead(lsf::asio::SharedSocket socket, std::string const& buffer) final;
    virtual void OnSocketPeerClose(lsf::asio::SharedSocket socket) final;

    // init logic
    virtual bool OnInitConfig() = 0;
    virtual bool OnInitSocket() = 0;
    virtual bool OnRun() { return true; }

    // message cut logic
    virtual bool OnGetSingleMessageFromStream(std::string const& buffer, size_t& pos, std::string& message);
    virtual bool OnPutSingleMessageIntoStream(std::string & buffer, std::string const& message);

    // connection handler
    bool ConnectionSend(lsf::asio::SharedSocket socket, std::string const& message);
    bool ConnectionSend(lsf::asio::SharedSocket socket, google::protobuf::Message const& proto_msg);
    void ConnectionClose(lsf::asio::SharedSocket socket);
    virtual void OnConnectionCreate(lsf::asio::SharedSocket socket) { }
    virtual void OnConnectionMessage(lsf::asio::SharedSocket socket, std::string& message) { }
    virtual void OnConnectionPeerClose(lsf::asio::SharedSocket socket) { }
    virtual void OnConnectionClose(lsf::asio::SharedSocket socket) { }

protected:
    conf::ENServiceType _service_type;
    BasicServer* _pserver = nullptr;
};

////////////////////////////////////////////////////////////
// BasicAcceptService
class BasicAcceptService : public BasicService {
public:
    friend class lsf::asio::ProactorSerivce;
    friend class BasicService;

public:
    BasicAcceptService(conf::ENServiceType service_type) : BasicService(service_type) {}
    virtual ~BasicAcceptService() {}

protected:
    // most of time you should not mess with these funcs
    void OnSocketAccept(lsf::asio::SharedSocket listen_socket, lsf::asio::SharedSocket socket);

    // init logic
    virtual bool OnInitConfig();
    virtual bool OnInitSocket();

protected:
    conf::AcceptService _service_config;
};

////////////////////////////////////////////////////////////
// BasicService
class BasicConnectService : public BasicService {
public:
    using conn_scok_type = std::vector<lsf::asio::SharedSocket>;
    using BasicService::ConnectionSend;
    static const size_t DEF_CONN_CHECK_INTERVAL = 10*1000; // milli seconds
    static const size_t DEF_CONNECT_TIMEOUT = 5*1000; // milli seconds
    static const size_t DEF_KEEP_SAME_INDEX_INTERVAL = 100; // milli seconds
    friend class lsf::asio::ProactorSerivce;
    friend class BasicService;

public:
    BasicConnectService(conf::ENServiceType service_type) : BasicService(service_type) {}
    virtual ~BasicConnectService() {}

    bool ConnectionSendBySequence(google::protobuf::Message const& message);
    bool ConnectionSendBySequence(std::string const& message);
    bool ConnectionSendByIndex(size_t index, google::protobuf::Message const& message);
    bool ConnectionSendByIndex(size_t index, std::string const& message);

protected:
    // most of time you should not mess with these funcs
    virtual void OnSocketConnect(lsf::asio::SharedSocket socket, lsf::asio::SockAddr const& sockaddr, size_t index) final;
    virtual void OnSocketConnectFail(lsf::asio::SharedSocket socket, lsf::asio::SockAddr const& sockaddr) final;

    // init logic
    virtual bool OnInitConfig();
    virtual bool OnInitSocket();
    virtual void OnConnectionCheck();

protected:
    size_t _last_random_send_index = 0;
    uint64_t _last_send_time = 0;
    conf::ConnectService _service_config;
    conn_scok_type _conn_scok;
};

// vim:ts=4:sw=4:et:ft=cpp:
