// File:        basic_sockaddr.hpp
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
#include "lsf/basic/string_ext.hpp"
#include "lsf/asio/detail/basic_address.hpp"

namespace lsf {
namespace asio {
namespace detail {

static const std::string DEF_DELIMIT = "|";

////////////////////////////////////////////////////////////
// BasicSockAddr
////////////////////////////////////////////////////////////
template<typename TransLayerProtocol>
class BasicSockAddr
{
public:
    typedef union {
        sockaddr        base;
        sockaddr_in     v4;
        sockaddr_in6    v6;
    }   sockaddr_type;

    typedef TransLayerProtocol                      proto_type;
    typedef typename proto_type::net_layer_proto    net_layer_proto;
    typedef BasicAddress<net_layer_proto>           address_type;


public:
    ////////////////////////////////////////////////////////////
    static BasicSockAddr Any(proto_type proto = proto_type::V4()) { 
        if (proto.domain() == AF_INET) return BasicSockAddr(proto_type::V4());
        else                           return BasicSockAddr(proto_type::V6());
    }

    ////////////////////////////////////////////////////////////
    BasicSockAddr(proto_type proto = proto_type::V4()) {
        if (proto.domain() == AF_INET) *this = BasicSockAddr(address_type(net_layer_proto::V4()), 0);
        else                           *this = BasicSockAddr(address_type(net_layer_proto::V6()), 0);
    }

    BasicSockAddr(address_type const & addr , uint16_t port) {
        if (addr.IsV4()) {
            _sockaddr.v4.sin_family = AF_INET;
            _sockaddr.v4.sin_port   = ::htons(port);
            ::memcpy(&_sockaddr.v4.sin_addr, &addr._addr, 4);
        }
        else {
            _sockaddr.v6.sin6_family = AF_INET6;
            _sockaddr.v6.sin6_port   = ::htons(port);
            _sockaddr.v6.sin6_flowinfo = 0;
            ::memcpy(&_sockaddr.v6.sin6_addr, &addr._addr, 16);
        }
    }

    BasicSockAddr(sockaddr const * paddr) {
        memcpy(&_sockaddr, paddr, sizeof(_sockaddr));
    }

    BasicSockAddr(std::string const & str) : BasicSockAddr(
                address_type(basic::StringExt::SplitAndGet(str, DEF_DELIMIT, 0)),
                basic::TypeCast<uint16_t>(basic::StringExt::SplitAndGet(str, DEF_DELIMIT, 1)))
    { }

    ////////////////////////////////////////////////////////////
    // member funcs
    std::string ToString() const {
        return GetAddress().ToString() + DEF_DELIMIT + basic::TypeCast< std::string>(GetPort());
    }

    uint16_t GetPort() const { 
        if (IsV4()) return ::ntohs(_sockaddr.v4.sin_port);
        else        return ::ntohs(_sockaddr.v6.sin6_port);
    }

    address_type GetAddress() const {
        if (IsV4()) return address_type(_sockaddr.v4.sin_addr);
        else        return address_type(_sockaddr.v6.sin6_addr);
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
                   ::memcmp(&_sockaddr.v6.sin6_addr, &_sockaddr.v6.sin6_addr, sizeof(_sockaddr.v6.sin6_addr));
    }

    bool operator!=(BasicSockAddr const & rhs) const { return !(*this == rhs); }

    sockaddr *       Data()       { return &_sockaddr.base; }
    sockaddr const * Data() const { return &_sockaddr.base; }

    size_t DataSize() const { 
        if (IsV4()) return sizeof(_sockaddr.v4);
        else        return sizeof(_sockaddr.v6);
    }

public:
    sockaddr_type   _sockaddr;
};

} // end of namespace detail
} // end of namespace asio
} // end of namespace lsf

// vim:ts=4:sw=4:et:ft=cpp:
