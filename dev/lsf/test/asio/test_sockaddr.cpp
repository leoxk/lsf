// File:        test_sockaddr.cpp
// Description: ---
// Notes:       ---
// Author:      leoxiang <leoxiang727@qq.com>
// Revision:    2015-08-25 by leoxiang

#include "lsf/basic/unit_test.hpp"
#include "lsf/asio/sockaddr.hpp"

using namespace std;
using namespace lsf::asio;
using namespace lsf::basic;

LSF_TEST_CASE(test_sockaddr_v4_and_v6) {
    LSF_ASSERT(SockAddr().IsV4());
    LSF_ASSERT(SockAddr(proto::v4).IsV4());
    LSF_ASSERT(SockAddr(proto::v6).IsV6());

    // constructor
    LSF_ASSERT(SockAddr().ToString() == "0.0.0.0|0");
    LSF_ASSERT(SockAddr(proto::v6).ToString() == "::|0");

    std::cout << SockAddr("192.168.1.1|32").ToString() << std::endl;
    LSF_ASSERT(SockAddr("192.168.1.1|32").ToString() == "192.168.1.1|32");
    LSF_ASSERT(SockAddr(Address(AF_INET6, "::ff:192.168.1.1"), 32).ToString() == "::ff:c0a8:101|32");

    SockAddr sockaddr("192.168.1.1|32");
    SockAddr sockaddr2 = sockaddr;
    LSF_ASSERT(SockAddr(sockaddr).ToString() == "192.168.1.1|32");
    LSF_ASSERT(SockAddr(sockaddr2).ToString() == "192.168.1.1|32");

    // static funcs
    LSF_ASSERT(SockAddr::Any().ToString() == "0.0.0.0|0");
    LSF_ASSERT(SockAddr::Any(proto::v6).ToString() == "::|0");
}

int main(int argc, char **argv) { LSF_TEST_ALL(argc, argv); }

// vim:ts=4:sw=4:et:ft=cpp:
