// File:        test_tcp.cpp
// Description: ---
// Notes:       ---
// Author:      leoxiang <leoxiang727@qq.com>
// Revision:    2012-06-08 by leoxiang

#include "lsf/basic/unit_test.hpp"
#include "lsf/basic/type_cast.hpp"
#include "lsf/asio/socket.hpp"
#include "lsf/util/random.hpp"

using namespace std;
using namespace lsf::asio;
using namespace lsf::util;
using namespace lsf::basic;

LSF_TEST_CASE(test_shared_socket) {
    SharedSocket sock, sock_v4, sock_v6;
    LSF_ASSERT(!sock);
    LSF_ASSERT(!sock_v4);
    LSF_ASSERT(!sock_v6);

    LSF_ASSERT(sock.use_count() == 0);
    sock = SharedSocket(proto::tcp);
    LSF_ASSERT(sock.use_count() == 1);
    sock_v4 = sock;
    LSF_ASSERT(sock.use_count() == 2);
    {
        SharedSocket tmp = sock;
        LSF_ASSERT(sock.use_count() == 3);
    }
    LSF_ASSERT(sock.use_count() == 2);

    sock_v6 = SharedSocket(proto::tcpv6);
    LSF_ASSERT(sock_v6.use_count() == 1);
}

LSF_TEST_CASE(test_tcp) {
    uint32_t listen_port = SingleRandom::Instance()->GetRand(15000, 16000);
    uint32_t bind_port = SingleRandom::Instance()->GetRand(17000, 18000);
    uint32_t wrong_port = SingleRandom::Instance()->GetRand(19000, 20000);

    // test listen
    Socket lsock(proto::tcp);
    LSF_ASSERT(lsock.Bind(SockAddr(Address::Any(), listen_port)));
    LSF_ASSERT(lsock.Listen());
    LSF_ASSERT(!lsock.IsConnect());
    LSF_ASSERT(lsock.LocalSockAddr().ToString() == StringExt::Format("0.0.0.0|%u", listen_port));

    // test socket create
    Socket socket(proto::tcp);
    LSF_ASSERT(socket.IsV4());
    LSF_ASSERT(socket.LocalSockAddr() == SockAddr::Any());
    LSF_ASSERT(socket.RemoteSockAddr() == SockAddr::Any());
    LSF_ASSERT(!socket.IsConnect());

    // test bind
    LSF_ASSERT(socket.Bind(SockAddr(Address::Any(), bind_port)));
    LSF_ASSERT(socket.LocalSockAddr() == SockAddr(Address::Any(), bind_port));
    LSF_ASSERT(socket.RemoteSockAddr() == SockAddr::Any());

    // test connect
    LSF_ASSERT(socket.Connect(SockAddr(Address::Any(), listen_port)));
    LSF_ASSERT(socket.IsConnect());
    LSF_ASSERT(socket.LocalSockAddr() == SockAddr(Address::Loopback(), bind_port));
    LSF_ASSERT(socket.RemoteSockAddr() == SockAddr(Address::Loopback(), listen_port));

    // test accept
    auto paccept_socket = lsock.Accept();
    LSF_ASSERT(paccept_socket);
    LSF_ASSERT(paccept_socket->IsConnect());
    LSF_ASSERT(paccept_socket->LocalSockAddr() == SockAddr(Address::Loopback(), listen_port));
    LSF_ASSERT(paccept_socket->RemoteSockAddr() == SockAddr(Address::Loopback(), bind_port));
    LSF_ASSERT(paccept_socket->LocalSockAddr() == socket.RemoteSockAddr());
    LSF_ASSERT(paccept_socket->RemoteSockAddr() == socket.LocalSockAddr());
    LSF_ASSERT(paccept_socket->SockFd() != socket.SockFd());
    socket.ShutDown();
    LSF_ASSERT(!socket.IsConnect());
    delete paccept_socket;

    // test connect wrong addr
    LSF_ASSERT(!socket.Connect(SockAddr(Address(AF_INET, "127.0.0.1"), wrong_port)));
}

LSF_TEST_CASE(test_udp) {
    Socket socket(proto::udp);
    LSF_ASSERT(socket.IsV4());
    LSF_ASSERT(socket.LocalSockAddr() == SockAddr::Any());
    LSF_ASSERT(socket.RemoteSockAddr() == SockAddr::Any());

    // test bind
    LSF_ASSERT(socket.Bind(SockAddr(Address(), 18888)));
    LSF_ASSERT(socket.LocalSockAddr() == SockAddr(Address(), 18888));
    LSF_ASSERT(socket.RemoteSockAddr() == SockAddr::Any());

    // test connect
    LSF_ASSERT(socket.Connect(SockAddr(Address::Any(), 19999)));
    LSF_ASSERT(socket.LocalSockAddr() == SockAddr(Address::Loopback(), 18888));
    LSF_ASSERT(socket.RemoteSockAddr() == SockAddr(Address::Loopback(), 19999));
}

int main(int argc, char **argv) { LSF_TEST_ALL(argc, argv); }

// vim:ts=4:sw=4:et:ft=cpp:
