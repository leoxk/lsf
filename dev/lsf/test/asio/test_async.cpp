// File:        test_asio.cpp
// Description: ---
// Notes:       ---
// Author:      leoxiang <leoxiang727@qq.com>
// Revision:    2015-05-19 by leoxiang

#include <functional>
#include "lsf/basic/unit_test.hpp"
#include "lsf/util/random.hpp"
#include "lsf/asio/async.hpp"

using namespace std;
using namespace lsf::asio;
using namespace lsf::basic;
using namespace lsf::util;

uint32_t listen_port = SingleRandom::Instance()->GetRand(15000, 16000);
uint32_t invalid_port = 65535;
string content = "this is async message";

////////////////////////////////////////////////////////////
void OnTimerFunc(int timer_fd) {
    static int counter = 0;
    counter++;

    if (counter >= 100) {
        counter = 0;
        IOService::Instance()->SetExit();
    }
}

////////////////////////////////////////////////////////////
void OnPeerCloseFunc(SharedSocket server_socket, SharedSocket client_socket) {
    LSF_ASSERT(!server_socket->IsConnect());
    LSF_ASSERT(!client_socket->IsConnect());

    // test timer
    LSF_ASSERT(IOService::Instance()->AsyncAddTimerOnce(1, OnTimerFunc) >= 0);
    LSF_ASSERT(IOService::Instance()->AsyncAddTimerMulti(1, 98, OnTimerFunc) >= 0);
    LSF_ASSERT(IOService::Instance()->AsyncAddTimerOnce(500, OnTimerFunc) >= 0);
}

////////////////////////////////////////////////////////////
void OnRecvFunc(SharedSocket server_socket, std::string const& buffer, SharedSocket client_socket) {
    // test msg
    LSF_ASSERT(buffer == content);
    LSF_ASSERT(server_socket->IsConnect());
    LSF_ASSERT(client_socket->IsConnect());

    // client close conn
    client_socket->ShutDown();
}

////////////////////////////////////////////////////////////
void OnSendFunc(SharedSocket client_socket, std::string const& buffer, SharedSocket server_socket) {
    // test address
    LSF_ASSERT(server_socket->RemoteSockAddr() == client_socket->LocalSockAddr());
    LSF_ASSERT(server_socket->LocalSockAddr()  == client_socket->RemoteSockAddr());
    LSF_ASSERT(server_socket->IsConnect());
    LSF_ASSERT(client_socket->IsConnect());

    // read data
    LSF_ASSERT(IOService::Instance()->AsyncRead(server_socket,
                                       std::bind(OnRecvFunc, std::placeholders::_1, std::placeholders::_2, client_socket),
                                       std::bind(OnPeerCloseFunc, std::placeholders::_1, client_socket)));
}

////////////////////////////////////////////////////////////
void OnAcceptFunc(SharedSocket listen_socket, SharedSocket server_socket, SharedSocket client_socket) {
    // test listen address
    LSF_ASSERT(listen_socket->LocalSockAddr() == SockAddr(Address::Any(), listen_port));
    LSF_ASSERT(server_socket->IsConnect());
    LSF_ASSERT(client_socket->IsConnect());
    LSF_ASSERT(!listen_socket->IsConnect());

    // test address
    LSF_ASSERT(server_socket->RemoteSockAddr() == client_socket->LocalSockAddr());
    LSF_ASSERT(server_socket->LocalSockAddr()  == client_socket->RemoteSockAddr());

    // send msg
    LSF_ASSERT(IOService::Instance()->AsyncWrite(client_socket, content,
                std::bind(OnSendFunc, std::placeholders::_1, std::placeholders::_2, server_socket)));
}

////////////////////////////////////////////////////////////
void OnConnectFunc(SharedSocket client_socket, SockAddr const& sockaddr, SharedSocket listen_socket) {
    // test listen address
    LSF_ASSERT(client_socket->CheckSockError());
    LSF_ASSERT(listen_socket->LocalSockAddr() == SockAddr(Address::Any(), listen_port));
    LSF_ASSERT(client_socket->IsConnect());
    LSF_ASSERT(!listen_socket->IsConnect());

    // async accept
    LSF_ASSERT(IOService::Instance()->AsyncAccept(listen_socket, std::bind(OnAcceptFunc, std::placeholders::_1, std::placeholders::_2, client_socket)));
}

////////////////////////////////////////////////////////////
void OnConnectFailFunc(SharedSocket client_socket, SockAddr const& sockaddr, SharedSocket listen_socket) {
    // test error
    LSF_ASSERT(client_socket->ErrString() == "Connection refused");
    LSF_ASSERT(client_socket.use_count() == 2);
    LSF_ASSERT(listen_socket.use_count() == 4);
    LSF_ASSERT(!client_socket->IsConnect());
    LSF_ASSERT(!listen_socket->IsConnect());

    // async connect again
    SharedSocket new_socket(proto::tcp);
    LSF_ASSERT(IOService::Instance()->AsyncConnect(new_socket, SockAddr(Address::Any(), listen_port),
                                   std::bind(OnConnectFunc, std::placeholders::_1, std::placeholders::_2, listen_socket),
                                   std::bind(OnConnectFailFunc, std::placeholders::_1, std::placeholders::_2, listen_socket)));
}

////////////////////////////////////////////////////////////
void Process() {
    LSF_ASSERT(IOService::Instance()->Driver()->GetRegisterEventSize() == 0);
    // listen
    SharedSocket listen_socket(proto::tcp);
    LSF_ASSERT(listen_socket->Bind(Address::Any(), listen_port));
    LSF_ASSERT(listen_socket->Listen());
    LSF_ASSERT(!listen_socket->IsConnect());

    // async connect
    LSF_ASSERT(IOService::Instance()->AsyncConnect(SharedSocket(proto::tcp), Address::Any(), invalid_port,
                                   std::bind(OnConnectFunc, std::placeholders::_1, std::placeholders::_2, listen_socket),
                                   std::bind(OnConnectFailFunc, std::placeholders::_1, std::placeholders::_2, listen_socket)));

    IOService::Instance()->Run();
    IOService::Instance()->AsyncCancel(listen_socket);
    listen_socket.reset();

    LSF_ASSERT(IOService::Instance()->Driver()->GetRegisterEventSize() == 0);
    LSF_ASSERT(IOService::Instance()->Queue().GetReadCompletionMap().size() == 0);
    LSF_ASSERT(IOService::Instance()->Queue().GetWriteCompletionMap().size() == 0);
    LSF_ASSERT(IOService::Instance()->Queue().GetRawCompletionMap().size() == 0);
}

LSF_TEST_CASE(test_asio_with_poll) {
    IOService::UsePoll();
    Process();
}

LSF_TEST_CASE(test_asio_with_epoll) {
    IOService::UseEpoll();
    Process();
}

int main(int argc, char **argv) { LSF_TEST_ALL(argc, argv); }

// vim:ts=4:sw=4:et:
