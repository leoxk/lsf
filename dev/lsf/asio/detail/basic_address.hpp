// File:        basic_address.hpp
// Description: ---
// Notes:       ---
// Author:      leoxiang <leoxiang727@qq.com>
// Revision:    2012-06-07 by leoxiang

#pragma once

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <string>
#include <ostream>
#include <cstring>

namespace lsf {
namespace asio {
namespace detail {

////////////////////////////////////////////////////////////
// BasicAddress
////////////////////////////////////////////////////////////
// forward declare
template <typename ProtoType>
class BasicSockAddr;

class BasicAddress {
public:
    typedef union {
        in_addr v4;
        in6_addr v6;
    } addr_type;

    template <typename ProtoType>
    friend class BasicSockAddr;

public:
    ////////////////////////////////////////////////////////////
    static BasicAddress Any(int domain = AF_INET) { return BasicAddress(domain); }

    static BasicAddress Loopback(int domain = AF_INET) {
        if (domain == AF_INET)
            return BasicAddress(AF_INET, "127.0.0.1");
        else
            return BasicAddress(AF_INET6, "::1");
    }

    ////////////////////////////////////////////////////////////
    BasicAddress(int domain = AF_INET, std::string const& ip_str = "") : _domain(domain) {
        if (domain == AF_INET) {
            if (!ip_str.empty())
                inet_pton(AF_INET, ip_str.c_str(), &_addr.v4);
            else
                inet_pton(AF_INET, "0.0.0.0", &_addr.v4);
        } else {
            if (!ip_str.empty())
                inet_pton(AF_INET6, ip_str.c_str(), &_addr.v6);
            else
                inet_pton(AF_INET6, "::", &_addr.v6);
        }
    }

    BasicAddress(std::string const& ip_str) : _domain(AF_INET) {
        if (inet_pton(AF_INET, ip_str.c_str(), &_addr.v4) <= 0) {
            _domain = AF_INET6;
            inet_pton(AF_INET6, ip_str.c_str(), &_addr.v6);
        }
    }

    BasicAddress(in6_addr const& addr) : _domain(AF_INET6) { ::memcpy(&_addr.v6, &addr, sizeof(_addr.v6)); }

    BasicAddress(in_addr const& addr) : _domain(AF_INET) { ::memcpy(&_addr.v4, &addr, sizeof(_addr.v4)); }

    ////////////////////////////////////////////////////////////
    // mem funcs
    std::string ToString() const {
        char tmp[128];
        if (_domain == AF_INET)
            return inet_ntop(AF_INET, &_addr.v4, tmp, sizeof(tmp));
        else
            return inet_ntop(AF_INET6, &_addr.v6, tmp, sizeof(tmp));
    }

    bool operator==(BasicAddress const& rhs) const {
        if (_domain != rhs._domain) return false;

        if (_domain == AF_INET)
            return ::memcmp(&_addr.v4, &rhs._addr.v4, sizeof(_addr.v4)) == 0;
        else
            return ::memcmp(&_addr.v6, &rhs._addr.v6, sizeof(_addr.v4)) == 0;
    }
    bool operator!=(BasicAddress const& rhs) const { return !(*this == rhs); }

    bool IsV4() const { return _domain == AF_INET; }
    bool IsV6() const { return _domain == AF_INET6; }

private:
    int _domain;
    addr_type _addr;  // network byte order
};

}  // end of namespace detail
}  // end of namespace asio
}  // end of namespace lsf

// vim:ts=4:sw=4:et:ft=cpp:
