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
// DummyNetLayerProtoType
////////////////////////////////////////////////////////////
class DummyNetLayerProtoType
{
public:
    static DummyNetLayerProtoType V4() { return DummyNetLayerProtoType(); }
    static DummyNetLayerProtoType V6() { return DummyNetLayerProtoType(); }
    int domain()   const { return 0; }
    bool operator==(DummyNetLayerProtoType const & rhs) const { return true; }
    bool operator!=(DummyNetLayerProtoType const & rhs) const { return false; }
};

////////////////////////////////////////////////////////////
// BasicAddress
////////////////////////////////////////////////////////////
// forward declare
template<typename TransLayerProtoType> class BasicSockAddr;

template<typename NetLayerProtoType>
class BasicAddress
{
public:
    typedef NetLayerProtoType    proto_type;
    typedef union {
        in_addr     v4;
        in6_addr    v6;
    }                           addr_type;

    template<typename TransLayerProtoType> friend class BasicSockAddr;

public:
    ////////////////////////////////////////////////////////////
    static BasicAddress Any(proto_type type = proto_type::V4()) { return BasicAddress(type); }

    static BasicAddress Loopback(proto_type type = proto_type::V4()) {
        if (type == proto_type::V4()) return BasicAddress(proto_type::V4(), "127.0.0.1"); 
        else                          return BasicAddress(proto_type::V6(), "::1");
    }

    ////////////////////////////////////////////////////////////
    explicit BasicAddress(proto_type type = proto_type::V4()) 
        : _type(type) 
    {
        if (IsV4()) _addr.v4.s_addr = INADDR_ANY;
        else        _addr.v6 = in6addr_any;
    }

    BasicAddress(proto_type type, std::string const & ip_str) : _type(type)
    {
        if (IsV4()) inet_pton(AF_INET,  ip_str.c_str(), &_addr.v4);
        else        inet_pton(AF_INET6, ip_str.c_str(), &_addr.v6);
    }

    BasicAddress(std::string const & ip_str) : _type(proto_type::V4())
    {
        if (inet_pton(AF_INET,  ip_str.c_str(), &_addr.v4) <= 0)
        {
            _type = proto_type::V6();
            inet_pton(AF_INET6, ip_str.c_str(), &_addr.v6);
        }
    }

    BasicAddress(in6_addr const & addr) : _type(proto_type::V6()) { ::memcpy(&_addr.v6, &addr, sizeof(_addr.v6)); }

    BasicAddress(in_addr const & addr) : _type(proto_type::V4()) { ::memcpy(&_addr.v4, &addr, sizeof(_addr.v4)); }

    ////////////////////////////////////////////////////////////
    // mem funcs
    std::string ToString() const {
        char tmp[128];
        if (IsV4()) return inet_ntop(AF_INET,  &_addr.v4, tmp, sizeof(tmp));
        else        return inet_ntop(AF_INET6, &_addr.v6, tmp, sizeof(tmp));
    }

    bool operator==(BasicAddress const & rhs) const {
        if (_type != rhs._type) return false;

        if (IsV4())      
            return ::memcmp(&_addr.v4, &rhs._addr.v4, sizeof(_addr.v4)) == 0;
        else
            return ::memcmp(&_addr.v6, &rhs._addr.v6, sizeof(_addr.v4)) == 0;
    }
    bool operator!=(BasicAddress const & rhs) const { return !(*this == rhs); }
    
    bool IsV4() const { return _type == proto_type::V4(); }
    bool IsV6() const { return _type == proto_type::V6(); }

private:
    proto_type  _type;
    addr_type   _addr;      // network byte order
};

} // end of namespace detail
} // end of namespace asio
} // end of namespace lsf

// vim:ts=4:sw=4:et:ft=cpp:
