// File:        ip.hpp
// Description: ---
// Notes:       ---
// Author:      leoxiang <leoxiang727@qq.com>
// Revision:    2012-06-01 by leoxiang

#pragma once

#include "lsf/asio/detail/basic_address.hpp"

namespace lsf {
namespace asio {
namespace ip {

class Protocol
{
public:
    Protocol(int domain) : _domain(domain) { }

    static Protocol V4() { return Protocol(AF_INET ); }
    static Protocol V6() { return Protocol(AF_INET6); }

    int domain() const { return _domain; }

    bool operator==(Protocol const rhs) const { return _domain == rhs._domain; }
    bool operator!=(Protocol const rhs) const { return _domain != rhs._domain; }

private:
    int _domain;
};

namespace {

static const Protocol V4(AF_INET );
static const Protocol V6(AF_INET6);
    
} // end of anonymous namespace

typedef detail::BasicAddress<Protocol>  Address;

} // end of namespace ip
} // end of namespace asio
} // end of namespace lsf

// vim:ts=4:sw=4:et:ft=cpp:
