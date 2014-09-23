// File:        basic_sockaddr.hpp
// Description: ---
// Notes:       ---
// Author:      leoxiang <leoxiang@tencent.com>
// Revision:    2012-06-07 by leoxiang

#pragma once

#include "lsf/basic/type_cast.hpp"
#include "lsf/asio/ip.hpp"

namespace lsf {
namespace asio {
namespace detail {

template<typename Protocol>
class BasicSockAddr
{
public:
    typedef union {
        sockaddr        base;
        sockaddr_in     v4;
        sockaddr_in6    v6;
    }                               sockaddr_type;

    typedef Protocol                proto_type;

public:
    BasicSockAddr(proto_type proto = proto_type::V4()) {
        if (proto.domain() == AF_INET) *this = BasicSockAddr(ip::Address(ip::V4), 0);
        else                           *this = BasicSockAddr(ip::Address(ip::V6), 0);
    }

    BasicSockAddr(ip::Address const & addr , uint16_t port) {
        if (addr.IsV4()) {
            _sockaddr.v4.sin_family = AF_INET;
            _sockaddr.v4.sin_port   = ::htons(port);
            ::memcpy(&_sockaddr.v4.sin_addr,  addr.ToBytes(), 4);
        }
        else {
            _sockaddr.v6.sin6_family = AF_INET6;
            _sockaddr.v6.sin6_port   = ::htons(port);
            _sockaddr.v6.sin6_flowinfo = 0;
            _sockaddr.v6.sin6_scope_id = addr.GetScopeId();
            ::memcpy(&_sockaddr.v6.sin6_addr, addr.ToBytes(), 16);
        }
    }

    BasicSockAddr(sockaddr const * paddr) {
        memcpy(&_sockaddr, paddr, sizeof(_sockaddr));
    }

    BasicSockAddr(BasicSockAddr const & rhs) : _sockaddr(rhs._sockaddr) { }

    BasicSockAddr & operator=(BasicSockAddr const & rhs) {
        if (this == &rhs) return *this;
        _sockaddr = rhs._sockaddr;
        return *this;
    }

    // member funcs
    std::string ToString() const {
        return GetAddress().ToString() + "/" + basic::TypeCast< std::string>(GetPort());
    }

    uint16_t GetPort() const { 
        if (IsV4()) return ::ntohs(_sockaddr.v4.sin_port);
        else        return ::ntohs(_sockaddr.v6.sin6_port);
    }

    ip::Address GetAddress() const {
        if (IsV4()) 
            return ip::Address(ip::V4, &_sockaddr.v4.sin_addr);
        else        
            return ip::Address(ip::V6, &_sockaddr.v6.sin6_addr, 
                    _sockaddr.v6.sin6_scope_id);
    }

    bool IsV4() const { return _sockaddr.base.sa_family == AF_INET; }
    bool IsV6() const { return _sockaddr.base.sa_family == AF_INET6; }

    bool operator==(BasicSockAddr const &rhs) const {
        if (IsV4())
            return _sockaddr.v4.sin_family      == rhs._sockaddr.v4.sin_family &&
                   _sockaddr.v4.sin_addr.s_addr == rhs._sockaddr.v4.sin_addr.s_addr && 
                   _sockaddr.v4.sin_port        == rhs._sockaddr.v4.sin_port;
        else
            return _sockaddr.v6.sin6_family     == rhs._sockaddr.v6.sin6_family &&
                   _sockaddr.v6.sin6_port       == rhs._sockaddr.v6.sin6_port &&
                   _sockaddr.v6.sin6_flowinfo   == rhs._sockaddr.v6.sin6_flowinfo &&
                   _sockaddr.v6.sin6_scope_id   == rhs._sockaddr.v6.sin6_scope_id &&
                   ::memcmp(&_sockaddr.v6.sin6_addr, &_sockaddr.v6.sin6_addr, sizeof(_sockaddr.v6.sin6_addr));
    }

    bool operator!=(BasicSockAddr const & rhs) const {
        return !(*this == rhs);
    }

    sockaddr *       Data()       { return &_sockaddr.base; }
    sockaddr const * Data() const { return &_sockaddr.base; }

    size_t DataSize() const { 
        if (IsV4()) return sizeof(_sockaddr.v4);
        else        return sizeof(_sockaddr.v6);
    }

    // static funcs
    static BasicSockAddr Any(proto_type proto = proto_type::V4()) { 
        if (proto.domain() == AF_INET) return BasicSockAddr(proto_type::V4());
        else                           return BasicSockAddr(proto_type::V6());
    }

public:
    sockaddr_type   _sockaddr;
};

} // end of namespace detail
} // end of namespace asio
} // end of namespace lsf

// vim:ts=4:sw=4:et:ft=cpp:
