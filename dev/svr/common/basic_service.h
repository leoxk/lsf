// File:        basic_service.h
// Description: ---
// Notes:       ---
// Author:      leoxiang <leoxiang727@qq.com>
// Revision:    2015-06-17 by leoxiang

#pragma once

#include <vector>
#include <map>
#include "lsf/asio/tcp.hpp"
#include "lsf/asio/net.hpp"
#include "lsf/basic/noncopyable.hpp"
#include "lsf/basic/error.hpp"
#include "svr/proto/conf_deploy.pb.h"
#include "svr/proto/msg_base.pb.h"
#include "svr/common/basic_server.h"

////////////////////////////////////////////////////////////
// BasicService
class BasicService : public lsf::basic::NonCopyable {
public:
    BasicService(conf::ENServiceType service_type) : _service_type(service_type) {}

    // main routine
    bool Run(BasicServer* pserver);

    // // active action
    // virtual bool SendMesaage(std::string const & buffer);
    // virtual bool BroadcastMesaage(std::string const & buffer);
    //
    // async handler
    virtual bool OnSocketRead(lsf::asio::AsyncInfo& info);
    virtual bool OnSocketPeerClose(lsf::asio::AsyncInfo& info);

protected:
    // connection handler
    virtual bool OnConnectionCreate(lsf::asio::Socket socket);
    virtual bool OnConnectionMessage(lsf::asio::Socket socket, std::string& message);
    virtual bool OnConnectionPeerClose(lsf::asio::Socket socket);

    // init logic
    virtual bool OnInitConfig() = 0;
    virtual bool OnInitSocket() = 0;

protected:
    conf::ENServiceType _service_type;
    BasicServer* _pserver;
};

////////////////////////////////////////////////////////////
// BasicAcceptService
class BasicAcceptService : public BasicService {
public:
    typedef std::map<lsf::asio::Socket, msg::TcpHead> acct_sock_type;

public:
    BasicAcceptService(conf::ENServiceType service_type) : BasicService(service_type) {}
    bool OnSocketAccept(lsf::asio::AsyncInfo& info);
    bool SendMessage(std::string const & buffer, lsf::asio::Socket socket);
    bool BroadcastMessage(std::string const & buffer);

public:
    size_t ConnSize() const { return _acct_sock.size(); }

protected:
    virtual bool OnInitConfig();
    virtual bool OnInitSocket();

protected:
    conf::AcceptService _service_config;
    acct_sock_type _acct_sock;
};

////////////////////////////////////////////////////////////
// BasicService
class BasicConnectService : public BasicService {
public:
    typedef std::vector<lsf::asio::Socket> conn_scok_type;

public:
    BasicConnectService(conf::ENServiceType service_type) : BasicService(service_type) {}
    virtual bool OnSocketConnect(lsf::asio::AsyncInfo& info, size_t index);
    virtual bool OnSocketPeerClose(lsf::asio::AsyncInfo& info);

protected:
    virtual bool OnInitConfig();
    virtual bool OnInitSocket();
    virtual bool TryConnect(size_t index);
    virtual bool TryAsyncConnect(size_t index);

protected:
    conf::ConnectService _service_config;
    conn_scok_type _conn_scok;
};

// vim:ts=4:sw=4:et:ft=cpp:
