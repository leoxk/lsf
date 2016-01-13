// File:        sockaddr.hpp
// Description: ---
// Notes:       ---
// Author:      leoxiang <leoxiang727@qq.com>
// Revision:    2012-06-07 by leoxiang

#pragma once
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include "lsf/basic/type_cast.hpp"
#include "lsf/util/string_ext.hpp"
#include "lsf/asio/address.hpp"

namespace lsf {
namespace asio {

////////////////////////////////////////////////////////////
// SockAddr
////////////////////////////////////////////////////////////
namespace { static const char* DEF_DELIMIT = "|"; }

class SockAddr {
public:
    using sockaddr_type = union {
        sockaddr base;
        sockaddr_in v4;
        sockaddr_in6 v6;
    };

public:
    ////////////////////////////////////////////////////////////
    // static function
    static SockAddr Any(proto::Domain domain = proto::v4) { return SockAddr(domain); }

    ////////////////////////////////////////////////////////////
    // constructor
    SockAddr(proto::Domain domain = proto::v4) : SockAddr(Address(domain), 0) { }

    SockAddr(std::string const& str)
        : SockAddr(Address(util::StringExt::SplitGet(str, DEF_DELIMIT, 0)),
                   basic::TypeCast<uint16_t>(util::StringExt::SplitGet(str, DEF_DELIMIT, 1))) {}

    SockAddr(Address const& address, uint16_t port) : _domain(address.domain()) {
        if (IsV4()) {
            _sockaddr.v4.sin_family = AF_INET;
            _sockaddr.v4.sin_port = ::htons(port);
            ::memcpy(&_sockaddr.v4.sin_addr, &address._address, sizeof(_sockaddr.v4.sin_addr));
        } else {
            _sockaddr.v6.sin6_family = AF_INET6;
            _sockaddr.v6.sin6_port = ::htons(port);
            _sockaddr.v6.sin6_flowinfo = 0;
            ::memcpy(&_sockaddr.v6.sin6_addr, &address._address, sizeof(_sockaddr.v6.sin6_addr));
        }
    }

    SockAddr(sockaddr const* paddr) : _domain(paddr->sa_family) {
        ::memcpy(&_sockaddr, paddr, sizeof(_sockaddr));
    }

    ////////////////////////////////////////////////////////////
    // member function
    std::string ToString() const { return GetAddress().ToString() + DEF_DELIMIT + basic::TypeCast<std::string>(GetPort()); }

    uint16_t GetPort() const {
        if (IsV4()) return ::ntohs(_sockaddr.v4.sin_port);
        else        return ::ntohs(_sockaddr.v6.sin6_port);
    }
    Address GetAddress() const {
        if (IsV4()) return Address(_sockaddr.v4.sin_addr);
        else        return Address(_sockaddr.v6.sin6_addr);
    }

    bool IsV4() const { return _domain.IsV4(); }
    bool IsV6() const { return _domain.IsV6(); }

    bool operator==(SockAddr const& rhs) const {
        if (IsV4()) return _sockaddr.v4.sin_family == rhs._sockaddr.v4.sin_family &&
                           _sockaddr.v4.sin_addr.s_addr == rhs._sockaddr.v4.sin_addr.s_addr &&
                           _sockaddr.v4.sin_port == rhs._sockaddr.v4.sin_port;
        else        return _sockaddr.v6.sin6_family == rhs._sockaddr.v6.sin6_family &&
                           _sockaddr.v6.sin6_port == rhs._sockaddr.v6.sin6_port &&
                           _sockaddr.v6.sin6_flowinfo == rhs._sockaddr.v6.sin6_flowinfo &&
                           ::memcmp(&_sockaddr.v6.sin6_addr, &_sockaddr.v6.sin6_addr, sizeof(_sockaddr.v6.sin6_addr));
    }
    bool operator!=(SockAddr const& rhs) const { return !(*this == rhs); }

    sockaddr const* data() const { return &_sockaddr.base; }
    socklen_t size() const { return IsV4() ? sizeof(_sockaddr.v4) : sizeof(_sockaddr.v6); }

private:
    proto::Domain _domain;
    sockaddr_type _sockaddr;
};

}  // end of namespace asio
}  // end of namespace lsf

// vim:ts=4:sw=4:et:ft=cpp:
