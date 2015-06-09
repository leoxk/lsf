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
template<typename NetLayerProtocol>
class BasicAddress
{
public:
    typedef NetLayerProtocol    proto_type;
    typedef union {
        in_addr     v4;
        in6_addr    v6;
    }                           addr_type;

public:
    ////////////////////////////////////////////////////////////
    // without address
    explicit BasicAddress(proto_type type = proto_type::V4()) 
        : _type(type), _scope_id(0) 
    {
        if (IsV4()) _addr.v4.s_addr = INADDR_ANY;
        else        _addr.v6 = in6addr_any;
    }

    // with address
    BasicAddress(std::string const & ip_str, uint32_t scope_id = 0) : _type(proto_type::V4())
    {
        *this = BasicAddress(proto_type::V4(), ip_str, scope_id);
    }

    BasicAddress(char const * ip_str, uint32_t scope_id = 0) : _type(proto_type::V4())
    {
        *this = BasicAddress(proto_type::V4(), ip_str, scope_id);
    }

    BasicAddress(proto_type type, std::string const & ip_str, uint32_t scope_id = 0) : _type(type)
    {
        *this = BasicAddress(type, ip_str.c_str(), scope_id);
    }

    BasicAddress(proto_type type, char const * ip_str, uint32_t scope_id = 0) 
        : _type(type), _scope_id(scope_id) 
    {
        if (IsV4()) inet_pton(AF_INET,  ip_str, &_addr.v4);
        else        inet_pton(AF_INET6, ip_str, &_addr.v6);
    }

    BasicAddress(proto_type type, void const * bytes, uint32_t scope_id = 0)
        : _type(type), _scope_id(scope_id) 
    {
        if (IsV4()) ::memcpy(&_addr.v4, bytes, sizeof(_addr.v4));
        else        ::memcpy(&_addr.v6, bytes, sizeof(_addr.v6));
    }

    BasicAddress(BasicAddress const & rhs) 
        : _type(rhs._type), _addr(rhs._addr), _scope_id(rhs._scope_id) { }

    BasicAddress & operator=(BasicAddress const & rhs) {
        if (this == &rhs) return *this;

        _type       = rhs._type;
        _addr       = rhs._addr;
        _scope_id   = rhs._scope_id;
        return *this;
    }

    ////////////////////////////////////////////////////////////
    // mem funcs
    std::string ToString() const {
        char tmp[128];
        if (IsV4()) return inet_ntop(AF_INET,  &_addr.v4, tmp, sizeof(tmp));
        else        return inet_ntop(AF_INET6, &_addr.v6, tmp, sizeof(tmp));
    }

    void const * ToBytes() const { return &_addr; }

    bool operator==(BasicAddress const & rhs) const {
        if (_type != rhs._type) return false;

        if (IsV4())      
            return ::memcmp(&_addr.v4, &rhs._addr.v4, sizeof(_addr.v4)) == 0;
        else
            return ::memcmp(&_addr.v6, &rhs._addr.v6, sizeof(_addr.v4)) == 0 && _scope_id == rhs._scope_id;
    }

    bool operator!=(BasicAddress const & rhs) const {
        return !(*this == rhs);
    }
    
    bool IsV4() const { return _type == proto_type::V4(); }
    bool IsV6() const { return _type == proto_type::V6(); }

    uint32_t GetScopeId() const { return _scope_id; }
    void     SetScopeId(uint32_t scope_id) { _scope_id = scope_id; }

    ////////////////////////////////////////////////////////////
    // static funcs
    static BasicAddress Any(proto_type type = proto_type::V4()) { return BasicAddress(type); }

    static BasicAddress Loopback(proto_type type = proto_type::V4()) {
        if (type == proto_type::V4()) return BasicAddress(proto_type::V4(), "127.0.0.1"); 
        else                          return BasicAddress(proto_type::V6(), "::1");
    }

private:
    proto_type  _type;
    addr_type   _addr;      // network byte order
    uint32_t    _scope_id;  // only for V6
};

} // end of namespace detail
} // end of namespace asio
} // end of namespace lsf

// vim:ts=4:sw=4:et:ft=cpp:
