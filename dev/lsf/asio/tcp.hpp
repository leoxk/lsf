// File:        tcp.hpp
// Description: ---
// Notes:       ---
// Author:      leoxiang <leoxiang727@qq.com>
// Revision:    2012-06-07 by leoxiang

#pragma once

#include "lsf/asio/detail/basic_sockaddr.hpp"
#include "lsf/asio/detail/basic_socket.hpp"
#include "lsf/asio/detail/basic_listen_socket.hpp"
#include "lsf/asio/ip.hpp"

namespace lsf {
namespace asio {
namespace tcp {

class Protocol
{
public:
    typedef ip::Protocol net_layer_proto;

    static Protocol V4() { return Protocol(AF_INET,  SOCK_STREAM, IPPROTO_TCP); }
    static Protocol V6() { return Protocol(AF_INET6, SOCK_STREAM, IPPROTO_TCP); }

public:
    Protocol(int domain, int type, int protocol)
        : _domain(domain), _type(type), _protocol(protocol) { }

    int domain()   const { return _domain;   }
    int type()     const { return _type;     }
    int protocol() const { return _protocol; }

private:
    int  _domain;
    int  _type;
    int  _protocol;
};

namespace {

static const Protocol V4(AF_INET,  SOCK_STREAM, IPPROTO_TCP);
static const Protocol V6(AF_INET6, SOCK_STREAM, IPPROTO_TCP);

} // end of anonymous namespace

typedef detail::BasicSockAddr<Protocol>        SockAddr;

typedef detail::BasicSocket<Protocol>          Socket;

typedef detail::BasicListenSocket<Protocol>    ListenSocket;

} // end of namespace tcp
} // end of namespace asio
} // end of namespace lsf

// vim:ts=4:sw=4:et:ft=cpp:
