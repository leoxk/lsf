// File:        test_asio.cpp
// Description: ---
// Notes:       ---
// Author:      leoxiang <leoxiang727@qq.com>
// Revision:    2015-05-19 by leoxiang

#include <functional>  
#include "lsf/basic/unit_test.hpp"
#include "lsf/asio/tcp.hpp"
#include "lsf/asio/io_service.hpp"
#include "lsf/util/random.hpp"

using namespace std;
using namespace lsf::asio;
using namespace lsf::basic;
using namespace lsf::util;

uint32_t listen_port = SingleRandom::Instance()->GetRand(15000, 16000);
string content = "this is async message";
EpollService epoll_service;

bool OnPeerCloseFunc(AsyncInfo & info, tcp::Socket server_socket)
{
    LSF_ASSERT(info.fd == server_socket.GetSockFd());

    return true;
}

bool OnRecvFunc(AsyncInfo & info, tcp::Socket server_socket)
{
    LSF_ASSERT(info.fd == server_socket.GetSockFd());

    // test msg
    LSF_ASSERT(info.buffer == content);

    return true;
}

bool OnSendFunc(AsyncInfo & info, tcp::Socket server_socket)
{
    // test address
    tcp::Socket client_socket(info.fd);
    LSF_ASSERT(server_socket.RemoteSockAddr() == client_socket.LocalSockAddr());
    LSF_ASSERT(server_socket.LocalSockAddr() == client_socket.RemoteSockAddr());
    
    // read data
    LSF_ASSERT(server_socket.AsyncRead(epoll_service, 
                std::bind(OnRecvFunc, std::placeholders::_1, server_socket), 
                std::bind(OnPeerCloseFunc, std::placeholders::_1, server_socket)));

    // client shutdown
    client_socket.CloseAsync(epoll_service);
    return true;
}

bool OnAcceptFunc(AsyncInfo & info, tcp::Socket client_socket)
{
    // test listen address
    tcp::Socket listen_socket(info.fd);
    LSF_ASSERT(listen_socket.LocalSockAddr() == tcp::SockAddr(ip::Address::Any(), listen_port));

    // test address
    tcp::Socket server_socket(info.accept_fd);
    LSF_ASSERT(server_socket.RemoteSockAddr() == client_socket.LocalSockAddr());
    LSF_ASSERT(server_socket.LocalSockAddr() == client_socket.RemoteSockAddr());

    // send msg
    LSF_ASSERT(client_socket.AsyncWrite(epoll_service, content.c_str(), content.size(), 
                std::bind(OnSendFunc, std::placeholders::_1, server_socket)));
    
    return true;
}

bool OnConnectFunc(AsyncInfo & info, tcp::ListenSocket listen_socket)
{
    // test listen address
    LSF_ASSERT(listen_socket.LocalSockAddr() == tcp::SockAddr(ip::Address::Any(), listen_port));

    // async accept
    tcp::Socket socket(info.fd);
    LSF_ASSERT(listen_socket.AsyncAccept(epoll_service, std::bind(OnAcceptFunc, std::placeholders::_1, socket)));

    return true;
}

LSF_TEST_CASE(test_asio)
{
    // listen
    tcp::ListenSocket listen_socket;
    LSF_ASSERT(listen_socket.Bind(tcp::SockAddr(ip::Address::Any(), listen_port)));
    LSF_ASSERT(listen_socket.Listen());

    // async connect
    tcp::Socket socket;
    LSF_ASSERT(socket.AsyncConnect(epoll_service, tcp::SockAddr(ip::Address::Any(), listen_port),
                std::bind(OnConnectFunc, std::placeholders::_1, listen_socket)));

    epoll_service.Run();
}

int main(int argc, char **argv)
{
	LSF_TEST_ALL();
}



// vim:ts=4:sw=4:et:
