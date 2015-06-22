// File:        test_net.cpp
// Description: ---
// Notes:       ---
// Author:      leoxiang <leoxiang727@qq.com>
// Revision:    2012-06-01 by leoxiang

#include "lsf/basic/unit_test.hpp"
#include "lsf/asio/net.hpp"

using namespace std;
using namespace lsf::asio;
using namespace lsf::basic;

LSF_TEST_CASE(test_address) {
    // test v4
    Address ipv4(AF_INET, "127.0.0.1");

    LSF_ASSERT(Address(AF_INET, "127.0.0.1") == ipv4);
    LSF_ASSERT(Address(ipv4) == ipv4);
    LSF_ASSERT(ipv4.ToString() == "127.0.0.1");
    LSF_ASSERT(ipv4.IsV4());

    LSF_ASSERT(Address::Loopback().IsV4());
    LSF_ASSERT(Address::Any().IsV4());

    LSF_ASSERT(Address::Any().IsV4());
    LSF_ASSERT(Address("127.0.0.1").IsV4());

    // test v6
    Address ipv6(AF_INET6, "::1");

    LSF_ASSERT(Address(ipv6) == ipv6);
    LSF_ASSERT(Address(AF_INET6, "::1") == ipv6);
    LSF_ASSERT(ipv6.ToString() == "::1");
    LSF_ASSERT(ipv4.IsV4());

    // test auto detect
    LSF_ASSERT(Address("127.0.0.1").IsV4());
    LSF_ASSERT(Address("255.255.255.255").IsV4());
    LSF_ASSERT(Address("255.255.255.256").IsV6());
    LSF_ASSERT(Address("0.0.0.0").IsV4());
    LSF_ASSERT(Address("::1").IsV6());
    LSF_ASSERT(Address("1:0:0:0:0:0:0:8").IsV6());
    LSF_ASSERT(Address("0:0:0:0:0:FFFF:204.152.189.116").IsV6());
    LSF_ASSERT(Address("::ffff:255.255.255.255").IsV6());

    // test static funcs
    LSF_ASSERT(Address::Loopback(AF_INET).ToString() == "127.0.0.1");
    LSF_ASSERT(Address::Loopback(AF_INET6).ToString() == "::1");
    LSF_ASSERT(Address(AF_INET) == Address::Any(AF_INET));
    LSF_ASSERT(Address(AF_INET6) == Address::Any(AF_INET6));
    LSF_ASSERT(Address::Any(AF_INET).ToString() == "0.0.0.0");
    LSF_ASSERT(Address::Any(AF_INET6).ToString() == "::");
    std::cout << Address::Any(AF_INET6).ToString() << std::endl;
}

int main(int argc, char **argv) { LSF_TEST_ALL(argc, argv); }

// vim:ts=4:sw=4:et:ft=cpp:
