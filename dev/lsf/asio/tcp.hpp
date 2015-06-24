// File:        tcp.hpp
// Description: ---
// Notes:       ---
// Author:      leoxiang <leoxiang727@qq.com>
// Revision:    2012-06-07 by leoxiang

#pragma once

#include "lsf/asio/detail/basic_sockaddr.hpp"
#include "lsf/asio/detail/basic_socket.hpp"
#include "lsf/asio/detail/basic_listen_socket.hpp"
#include "lsf/asio/net.hpp"

namespace lsf {
namespace asio {
namespace tcp {

////////////////////////////////////////////////////////////
// Protocol
class Protocol {
public:
    constexpr static Protocol V4() { return Protocol(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, IPPROTO_TCP); }
    constexpr static Protocol V6() { return Protocol(AF_INET6, SOCK_STREAM | SOCK_NONBLOCK, IPPROTO_TCP); }

public:
    constexpr Protocol(int domain, int type, int protocol) : _domain(domain), _type(type), _protocol(protocol) {}

    int domain() const { return _domain; }
    int type() const { return _type; }
    int protocol() const { return _protocol; }

private:
    int _domain = AF_INET;
    int _type = SOCK_STREAM;
    int _protocol = IPPROTO_TCP;
};

// constexpr
constexpr Protocol V4 = Protocol::V4();
constexpr Protocol V6 = Protocol::V6();

// typedef
typedef detail::BasicSockAddr<Protocol> SockAddr;
typedef detail::BasicSocket<Protocol> Socket;
typedef detail::BasicListenSocket<Protocol> ListenSocket;

}  // end of namespace tcp
}  // end of namespace asio
}  // end of namespace lsf

// vim:ts=4:sw=4:et:ft=cpp:
