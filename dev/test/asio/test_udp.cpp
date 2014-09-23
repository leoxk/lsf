// File:        test_udp.cpp
// Description: ---
// Notes:       ---
// Author:      leoxiang <leoxiang@tencent.com>
// Revision:    2012-06-05 by leoxiang

#include "lsf/basic/unit_test.hpp"
#include "lsf/asio/udp.hpp"

using namespace std;
using namespace lsf::asio;

LSF_TEST_CASE(test_sockaddr_v4_and_v6)
{
    LSF_ASSERT(udp::SockAddr().IsV4());
    LSF_ASSERT(udp::SockAddr(udp::V4).IsV4());
    LSF_ASSERT(udp::SockAddr(udp::V6).IsV6());

    // constructor
    LSF_ASSERT(udp::SockAddr().ToString() == "0.0.0.0/0");
    LSF_ASSERT(udp::SockAddr(udp::V6).ToString() == "::/0");

    LSF_ASSERT(udp::SockAddr(ip::Address(ip::V4, "192.168.1.1"), 32).ToString() == "192.168.1.1/32");
    LSF_ASSERT(udp::SockAddr(ip::Address(ip::V6, "::ff:192.168.1.1"), 32).ToString() == "::ff:c0a8:101/32");

    udp::SockAddr  sockaddr(ip::Address(ip::V4, "192.168.1.1"), 32);
    udp::SockAddr  sockaddr2 = sockaddr;
    LSF_ASSERT(udp::SockAddr(sockaddr).ToString() == "192.168.1.1/32");
    LSF_ASSERT(udp::SockAddr(sockaddr2).ToString() == "192.168.1.1/32");

    // static funcs
    LSF_ASSERT(udp::SockAddr::Any().ToString() == "0.0.0.0/0");
    LSF_ASSERT(udp::SockAddr::Any(udp::V6).ToString() == "::/0");
}

LSF_TEST_CASE(test_sock_op_v4)
{
    udp::Socket socket;
    LSF_ASSERT(socket.IsV4());
    LSF_ASSERT(socket.LocalSockAddr() == udp::SockAddr::Any());
    LSF_ASSERT(socket.RemoteSockAddr() == udp::SockAddr::Any());

    cout << "sock recv buf is: " << socket.GetRecvBufLen() << endl;

    //test bind
    LSF_ASSERT(socket.Bind(udp::SockAddr(ip::Address(), 18888)));
    LSF_ASSERT(socket.LocalSockAddr() == udp::SockAddr(ip::Address(), 18888));
    LSF_ASSERT(socket.RemoteSockAddr() ==udp::SockAddr::Any());

    //test connect
    LSF_ASSERT(socket.Connect(udp::SockAddr(ip::Address::Any(), 19999)));
    LSF_ASSERT(socket.LocalSockAddr()  == udp::SockAddr(ip::Address::Loopback(), 18888));
    LSF_ASSERT(socket.RemoteSockAddr() == udp::SockAddr(ip::Address::Loopback(), 19999));

}

LSF_TEST_CASE(test_sock_v6)
{

}

LSF_TEST_CASE(test_sock_opt)
{

}

int main(int argc, char **argv)
{
	LSF_TEST_ALL();
}

// vim:ts=4:sw=4:et:ft=cpp:
