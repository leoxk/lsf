// File:        test_ip.cpp
// Description: ---
// Notes:       ---
// Author:      leoxiang <leoxiang727@qq.com>
// Revision:    2012-06-01 by leoxiang

#include "lsf/basic/unit_test.hpp"
#include "lsf/asio/ip.hpp"

using namespace std;
using namespace lsf::asio;
using namespace lsf::basic;

LSF_TEST_CASE(test_address)
{
    // test v4
    ip::Address ipv4(ip::V4, "127.0.0.1");

    LSF_ASSERT(ip::Address(ip::V4, "127.0.0.1") == ipv4);
    LSF_ASSERT(ip::Address(ipv4) == ipv4);
    LSF_ASSERT(ipv4.ToString() == "127.0.0.1");
    LSF_ASSERT(ipv4.IsV4());

    LSF_ASSERT(ip::Address::Loopback().IsV4());
    LSF_ASSERT(ip::Address::Any().IsV4());
    
    LSF_ASSERT(ip::Address::Any().IsV4());
    LSF_ASSERT(ip::Address("127.0.01").IsV4());

    // test v6
    ip::Address ipv6(ip::V6, "::1");

    LSF_ASSERT(ip::Address(ipv6) == ipv6);
    LSF_ASSERT(ip::Address(ip::V6, "::1") == ipv6);
    LSF_ASSERT(ipv6.ToString() == "::1");
    LSF_ASSERT(ipv4.IsV4());

    // test static funcs
    LSF_ASSERT(ip::Address::Loopback(ip::V4).ToString() == "127.0.0.1");
    LSF_ASSERT(ip::Address::Loopback(ip::V6).ToString() == "::1");
    LSF_ASSERT(ip::Address::Loopback(ip::V6).GetScopeId() == 0);
    LSF_ASSERT(ip::Address(ip::V4) == ip::Address::Any(ip::V4));
    LSF_ASSERT(ip::Address(ip::V6) == ip::Address::Any(ip::V6));
    LSF_ASSERT(ip::Address::Any(ip::V4).ToString() == "0.0.0.0");
    LSF_ASSERT(ip::Address::Any(ip::V6).ToString() == "::");
}

int main(int argc, char **argv)
{
	LSF_TEST_ALL();
}


// vim:ts=4:sw=4:et:ft=cpp:
