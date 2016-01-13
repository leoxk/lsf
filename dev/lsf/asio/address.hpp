// File:        address.hpp
// Description: ---
// Notes:       ---
// Author:      leoxiang <leoxiang727@qq.com>
// Revision:    2012-06-07 by leoxiang

#pragma once
#include <arpa/inet.h>
#include <string>
#include <cstring>
#include "lsf/asio/protocol.hpp"

namespace lsf {
namespace asio {

////////////////////////////////////////////////////////////
// Address
////////////////////////////////////////////////////////////
class Address {
public:
    const static size_t MAX_ADDRESS_NAME = 128;
    using address_type = union {
        in_addr v4;
        in6_addr v6;
    };
    friend class SockAddr;

public:
    ////////////////////////////////////////////////////////////
    // static function
    static Address Any(proto::Domain domain = proto::v4) { return Address(domain); }
    static Address Loopback(proto::Domain domain = proto::v4) { return domain == proto::v4 ? Address(AF_INET, "127.0.0.1") : Address(AF_INET6, "::1"); }

    ////////////////////////////////////////////////////////////
    // constructor
    Address(proto::Domain domain = proto::v4, std::string const& ip_str = "") : _domain(domain) {
        if (IsV4()) {
            if (!ip_str.empty())    inet_pton(AF_INET, ip_str.c_str(), &_address.v4);
            else                    inet_pton(AF_INET, "0.0.0.0", &_address.v4);
        } else {
            if (!ip_str.empty())    inet_pton(AF_INET6, ip_str.c_str(), &_address.v6);
            else                    inet_pton(AF_INET6, "::", &_address.v6);
        }
    }

    Address(std::string const& ip_str) : _domain(proto::v4) {
        if (inet_pton(AF_INET, ip_str.c_str(), &_address.v4) > 0) {
            _domain = proto::v4;
        }
        else if (inet_pton(AF_INET6, ip_str.c_str(), &_address.v6) > 0) {
            _domain = proto::v6;
        }
        else {
            *this = Any();
        }
    }

    Address(in_addr const& addr)  : _domain(proto::v4) { ::memcpy(&_address.v4, &addr, sizeof(_address.v4)); }
    Address(in6_addr const& addr) : _domain(proto::v6) { ::memcpy(&_address.v6, &addr, sizeof(_address.v6)); }

    ////////////////////////////////////////////////////////////
    // member function
    std::string ToString() const {
        char tmp[MAX_ADDRESS_NAME];
        if (IsV4()) return inet_ntop(AF_INET,  &_address.v4, tmp, sizeof(tmp));
        else        return inet_ntop(AF_INET6, &_address.v6, tmp, sizeof(tmp));
    }

    bool operator==(Address const& rhs) const {
        if (_domain != rhs._domain) return false;
        if (IsV4()) return ::memcmp(&_address.v4, &rhs._address.v4, sizeof(_address.v4)) == 0;
        else        return ::memcmp(&_address.v6, &rhs._address.v6, sizeof(_address.v4)) == 0;
    }
    bool operator!=(Address const& rhs) const { return !(*this == rhs); }

    bool IsV4() const { return _domain.IsV4(); }
    bool IsV6() const { return _domain.IsV6(); }

    proto::Domain domain() const { return _domain; }

    bool IsLoopback() const { return (IsV4() && ToString() == "127.0.0.1") || (IsV6() && ToString() == "::1"); }
    bool IsAny() const { return (IsV4() && ToString() == "0.0.0.0") || (IsV6() && ToString() == "::"); }
    bool IsPrivate() const {
        if (IsV6()) return false;
        uint32_t ip = ntohl(_address.v4.s_addr);
        return (ip >> 24) == 0xa ||
               (ip >> 20) == 0xac1 ||
               (ip >> 16) == 0xc0a8;
    }

private:
    proto::Domain _domain;
    address_type _address;  // network byte order
};

}  // end of namespace asio
}  // end of namespace lsf

// vim:ts=4:sw=4:et:ft=cpp:
