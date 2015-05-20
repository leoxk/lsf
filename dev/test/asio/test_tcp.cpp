// File:        test_tcp.cpp
// Description: ---
// Notes:       ---
// Author:      leoxiang <leoxiang727@qq.com>
// Revision:    2012-06-08 by leoxiang

#include "lsf/basic/unit_test.hpp"
#include "lsf/asio/tcp.hpp"
#include "lsf/util/random.hpp"
#include "lsf/basic/type_cast.hpp"

using namespace std;
using namespace lsf::asio;
using namespace lsf::util;
using namespace lsf::basic;

LSF_TEST_CASE(test_sockaddr_v4_and_v6)
{
    LSF_ASSERT(tcp::SockAddr().IsV4());
    LSF_ASSERT(tcp::SockAddr(tcp::V4).IsV4());
    LSF_ASSERT(tcp::SockAddr(tcp::V6).IsV6());

    // constructor
    LSF_ASSERT(tcp::SockAddr().ToString() == "0.0.0.0|0");
    LSF_ASSERT(tcp::SockAddr(tcp::V6).ToString() == "::|0");

    LSF_ASSERT(tcp::SockAddr(ip::Address(ip::V4, "192.168.1.1"), 32).ToString() == "192.168.1.1|32");
    LSF_ASSERT(tcp::SockAddr(ip::Address(ip::V6, "::ff:192.168.1.1"), 32).ToString() == "::ff:c0a8:101|32");

    tcp::SockAddr  sockaddr(ip::Address(ip::V4, "192.168.1.1"), 32);
    tcp::SockAddr  sockaddr2 = sockaddr;
    LSF_ASSERT(tcp::SockAddr(sockaddr).ToString() == "192.168.1.1|32");
    LSF_ASSERT(tcp::SockAddr(sockaddr2).ToString() == "192.168.1.1|32");

    // static funcs
    LSF_ASSERT(tcp::SockAddr::Any().ToString() == "0.0.0.0|0");
    LSF_ASSERT(tcp::SockAddr::Any(tcp::V6).ToString() == "::|0");
}

LSF_TEST_CASE(test_sock_op_v4)
{
    uint32_t listen_port = SingleRandom::Instance()->GetRand(15000, 16000);
    uint32_t bind_port = SingleRandom::Instance()->GetRand(17000, 18000);
    uint32_t wrong_port = SingleRandom::Instance()->GetRand(19000, 20000);

    // test listen
    tcp::ListenSocket lsock(tcp::SockAddr(ip::Address::Any(), listen_port));
    LSF_ASSERT(lsock.Listen());

    // test socket create
    tcp::Socket socket;
    LSF_ASSERT(socket.IsV4());
    LSF_ASSERT(socket.LocalSockAddr()  == tcp::SockAddr::Any());
    LSF_ASSERT(socket.RemoteSockAddr() == tcp::SockAddr::Any());

    // test bind
    LSF_ASSERT(socket.Bind(tcp::SockAddr(ip::Address::Any(), bind_port)));
    LSF_ASSERT(socket.LocalSockAddr()  == tcp::SockAddr(ip::Address::Any(), bind_port));
    LSF_ASSERT(socket.RemoteSockAddr() == tcp::SockAddr::Any());

    // test connect
    LSF_ASSERT(socket.Connect(tcp::SockAddr(ip::Address::Any(), listen_port)));
    LSF_ASSERT(socket.LocalSockAddr()  == tcp::SockAddr(ip::Address::Loopback(), bind_port));
    LSF_ASSERT(socket.RemoteSockAddr() == tcp::SockAddr(ip::Address::Loopback(), listen_port));

    // test 
    tcp::Socket accept_socket;
    LSF_ASSERT(lsock.Accept(accept_socket));
    LSF_ASSERT(accept_socket.LocalSockAddr()  == tcp::SockAddr(ip::Address::Loopback(), listen_port));
    LSF_ASSERT(accept_socket.RemoteSockAddr() == tcp::SockAddr(ip::Address::Loopback(), bind_port));

    LSF_ASSERT(socket.Close());

    // test connect wrong addr
    LSF_ASSERT(!socket.Connect(tcp::SockAddr(ip::Address(ip::V4, "127.0.0.1"), wrong_port)));
}

int main(int argc, char **argv)
{
    LSF_TEST_ALL();
}

// vim:ts=4:sw=4:et:ft=cpp:
