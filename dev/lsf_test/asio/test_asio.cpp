// File:        test_asio.cpp
// Description: ---
// Notes:       ---
// Author:      leoxiang <leoxiang727@qq.com>
// Revision:    2015-05-19 by leoxiang

#include <functional>
#include "lsf/basic/unit_test.hpp"
#include "lsf/asio/tcp.hpp"
#include "lsf/asio/net.hpp"
#include "lsf/util/random.hpp"

using namespace std;
using namespace lsf::asio;
using namespace lsf::basic;
using namespace lsf::util;

uint32_t listen_port = SingleRandom::Instance()->GetRand(15000, 16000);
string content = "this is async message";

////////////////////////////////////////////////////////////
bool OnTimerFunc(int timer_fd) {
    static int counter = 0;
    counter++;

    if (counter >= 100) {
        IOService::Instance()->SetExit();
    }
    return true;
}

////////////////////////////////////////////////////////////
void OnPeerCloseFunc(Socket socket, Socket client_socket) {
    // test timer
    LSF_ASSERT(IOService::Instance()->AsyncAddTimerSingle(1, OnTimerFunc));
    LSF_ASSERT(IOService::Instance()->AsyncAddTimerMulti(1, 98, OnTimerFunc));
    LSF_ASSERT(IOService::Instance()->AsyncAddTimerSingle(500, OnTimerFunc));
}

////////////////////////////////////////////////////////////
bool OnRecvFunc(Socket socket, std::string const& buffer, Socket client_socket) {
    // test msg
    LSF_ASSERT(buffer == content);

    // client close conn
    IOService::Instance()->AsyncClose(client_socket);

    return true;
}

////////////////////////////////////////////////////////////
bool OnSendFunc(Socket socket, std::string const& buffer, Socket server_socket) {
    // test address
    LSF_ASSERT(server_socket.RemoteSockAddr() == socket.LocalSockAddr());
    LSF_ASSERT(server_socket.LocalSockAddr() == socket.RemoteSockAddr());

    // read data
    LSF_ASSERT(IOService::Instance()->AsyncRead(server_socket,
                                       std::bind(OnRecvFunc, std::placeholders::_1, std::placeholders::_2, socket),
                                       std::bind(OnPeerCloseFunc, std::placeholders::_1, socket)));

    return true;
}

////////////////////////////////////////////////////////////
bool OnAcceptFunc(Socket socket, ListenSocket listen_socket, Socket client_socket) {
    // test listen address
    LSF_ASSERT(listen_socket.LocalSockAddr() == tcp::SockAddr(Address::Any(), listen_port));

    // test address
    LSF_ASSERT(socket.RemoteSockAddr() == client_socket.LocalSockAddr());
    LSF_ASSERT(socket.LocalSockAddr() == client_socket.RemoteSockAddr());

    // send msg
    LSF_ASSERT(IOService::Instance()->AsyncWrite(client_socket, content,
                std::bind(OnSendFunc, std::placeholders::_1, std::placeholders::_2, socket)));

    return true;
}

////////////////////////////////////////////////////////////
bool OnConnectFunc(Socket socket, SockAddr const& sockaddr, ListenSocket listen_socket) {
    // test listen address
    LSF_ASSERT(socket.GetSockError() == 0);
    LSF_ASSERT(listen_socket.LocalSockAddr() == tcp::SockAddr(Address::Any(), listen_port));

    // async accept
    LSF_ASSERT(IOService::Instance()->AsyncAccept(listen_socket, std::bind(OnAcceptFunc, std::placeholders::_1, std::placeholders::_2, socket)));

    return true;
}

////////////////////////////////////////////////////////////
void OnConnectFailFunc(Socket socket, SockAddr const& sockaddr, ListenSocket listen_socket) {
    // test error
    LSF_ASSERT(IOService::Instance()->ErrString() == "Connection refused");
    // std::cout << IOService::Instance()->ErrString() << std::endl;

    // async connect again
    Socket new_socket = tcp::Socket::CreateSocket();
    LSF_ASSERT(IOService::Instance()->AsyncConnect(new_socket, tcp::SockAddr(Address::Any(), listen_port),
                                   std::bind(OnConnectFunc, std::placeholders::_1, std::placeholders::_2, listen_socket),
                                   std::bind(OnConnectFailFunc, std::placeholders::_1, std::placeholders::_2, listen_socket)));
}

LSF_TEST_CASE(test_asio) {
    // listen
    ListenSocket listen_socket = tcp::ListenSocket::CreateListenSocket();
    LSF_ASSERT(listen_socket.Bind(tcp::SockAddr(Address::Any(), listen_port)));
    LSF_ASSERT(listen_socket.Listen());

    // async connect
    Socket socket = tcp::Socket::CreateSocket();
    LSF_ASSERT(IOService::Instance()->AsyncConnect(socket, tcp::SockAddr(Address::Any(), 65535),
                                   std::bind(OnConnectFunc, std::placeholders::_1, std::placeholders::_2, listen_socket),
                                   std::bind(OnConnectFailFunc, std::placeholders::_1, std::placeholders::_2, listen_socket)));

    IOService::Instance()->Run();
}

int main(int argc, char **argv) { LSF_TEST_ALL(argc, argv); }

// vim:ts=4:sw=4:et:
