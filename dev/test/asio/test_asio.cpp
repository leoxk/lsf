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

bool AcceptFunc(AsyncInfo & info, tcp::Socket client_socket)
{
    tcp::Socket listen_socket(info.fd);
    tcp::Socket accept_socket(info.accept_fd);
    std::cout << info.fd << " " << info.accept_fd << std::endl;

    LSF_ASSERT(listen_socket.LocalSockAddr() == tcp::SockAddr(ip::Address("192.168.7.172"), listen_port));
    LSF_ASSERT(accept_socket.RemoteSockAddr() == client_socket.LocalSockAddr());
    LSF_ASSERT(accept_socket.LocalSockAddr() == client_socket.RemoteSockAddr());
    std::cout << client_socket.GetSockFd() << " " << client_socket.LocalSockAddr().ToString() << " " << client_socket.RemoteSockAddr().ToString() << std::endl;
    std::cout << accept_socket.GetSockFd() << " " << accept_socket.LocalSockAddr().ToString() << " " << accept_socket.RemoteSockAddr().ToString() << std::endl;

    sleep(1000);

    return true;
}

LSF_TEST_CASE(test_asio)
{

    // listen
    tcp::ListenSocket listen_socket;
    LSF_ASSERT(listen_socket.Bind(tcp::SockAddr(ip::Address("192.168.7.172"), listen_port)));
    LSF_ASSERT(listen_socket.Listen());

    // connect
    tcp::Socket socket;
    std::cout << socket.GetSockFd() << std::endl;
    LSF_ASSERT(socket.Connect(tcp::SockAddr(ip::Address("192.168.7.172"), listen_port)));

    // async accept
    EpollIOService io_service;
    LSF_ASSERT(io_service.AsyncAccept(listen_socket, std::bind(AcceptFunc, std::placeholders::_1, socket)));
    std::cout << socket.GetSockFd() << " " << socket.LocalSockAddr().ToString() << " " << socket.RemoteSockAddr().ToString() << std::endl;

    io_service.Run();
}

int main(int argc, char **argv)
{
	LSF_TEST_ALL();
}



// vim:ts=4:sw=4:et:
