// File:        udp.hpp
// Description: ---
// Notes:       ---
// Author:      leoxiang <leoxiang727@qq.com>
// Revision:    2012-06-07 by leoxiang

#pragma once

#include "lsf/asio/detail/basic_sockaddr.hpp"
#include "lsf/asio/detail/basic_socket.hpp"
#include "lsf/asio/net.hpp"

namespace lsf {
namespace asio {
namespace udp {

////////////////////////////////////////////////////////////
// Protocol
class Protocol {
public:
    static Protocol V4() { return Protocol(AF_INET, SOCK_DGRAM | SOCK_NONBLOCK, IPPROTO_UDP); }
    static Protocol V6() { return Protocol(AF_INET6, SOCK_DGRAM | SOCK_NONBLOCK, IPPROTO_UDP); }

public:
    constexpr Protocol(int domain, int type, int protocol) : _domain(domain), _type(type), _protocol(protocol) {}

    int domain() const { return _domain; }
    int type() const { return _type; }
    int protocol() const { return _protocol; }

private:
    int _domain;
    int _type;
    int _protocol;
};

// constexpr
constexpr Protocol V4(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
constexpr Protocol V6(AF_INET6, SOCK_DGRAM, IPPROTO_UDP);

// typedef
typedef detail::BasicSockAddr<Protocol> SockAddr;
typedef detail::BasicSocket<Protocol> Socket;

}  // end of namespace udp
}  // end of namespace asio
}  // end of namespace lsf

// vim:ts=4:sw=4:et:ft=cpp:
