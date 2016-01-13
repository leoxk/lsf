// File:        protocol.hpp
// Description: ---
// Notes:       ---
// Author:      leoxiang <leoxiang727@qq.com>
// Revision:    2015-08-25 by leoxiang

#pragma once
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

namespace lsf {
namespace asio {
namespace proto {

////////////////////////////////////////////////////////////
class Domain {
public:
    constexpr Domain(int domain) : _domain(domain) {}

    int domain() const { return _domain; }

    bool IsV4() const { return _domain == AF_INET; }
    bool IsV6() const { return _domain == AF_INET6; }
    bool operator==(Domain const& rhs) const { return _domain == rhs._domain; }
    bool operator!=(Domain const& rhs) const { return !(*this == rhs); }

private:
    int _domain;
};

////////////////////////////////////////////////////////////
class Protocol {
public:
    constexpr Protocol(Domain domain, int type, int protocol) :
        _domain(domain), _type(type), _protocol(protocol) {}

    Domain domain() const { return _domain; }
    int type() const { return _type; }
    int protocol() const { return _protocol; }

    bool IsV4() const { return _domain.IsV4(); }
    bool IsV6() const { return _domain.IsV6(); }
    bool operator==(Protocol const& rhs) const { return _domain == rhs._domain && _type == rhs._type && _protocol == rhs._protocol; }
    bool operator!=(Protocol const& rhs) const { return !(*this == rhs); }

private:
    Domain _domain;
    int _type;
    int _protocol;
};

////////////////////////////////////////////////////////////
namespace {
    Domain v4 = { AF_INET  };
    Domain v6 = { AF_INET6 };
    Protocol tcp   = { v4, SOCK_STREAM | SOCK_NONBLOCK, IPPROTO_TCP };
    Protocol udp   = { v4, SOCK_DGRAM  | SOCK_NONBLOCK, IPPROTO_UDP };
    Protocol tcpv6 = { v6, SOCK_STREAM | SOCK_NONBLOCK, IPPROTO_TCP };
    Protocol udpv6 = { v6, SOCK_DGRAM  | SOCK_NONBLOCK, IPPROTO_UDP };
}

} // end namespace proto
} // end namespace asio
} // end namespace lsf

// vim:ts=4:sw=4:et:ft=cpp:
