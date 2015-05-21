// File:        test_asio.cpp
// Description: ---
// Notes:       ---
// Author:      leoxiang <leoxiang727@qq.com>
// Revision:    2015-05-19 by leoxiang

#include "lsf/basic/unit_test.hpp"
#include "lsf/asio/tcp.hpp"
#include "lsf/asio/io_service.hpp"
#include "lsf/util/random.hpp"

using namespace std;
using namespace lsf::asio;
using namespace lsf::basic;
using namespace lsf::util;

bool AcceptFunc(AsyncInfo & info)
{
    std::cout << "get a accept: " << info.fd << std::endl;
    return true;
}

LSF_TEST_CASE(test_asio)
{
    uint32_t listen_port = SingleRandom::Instance()->GetRand(15000, 16000);

    // listen
    tcp::ListenSocket listen_socket;
    LSF_ASSERT(listen_socket.Bind(tcp::SockAddr(ip::Address::Any(), listen_port)));
    LSF_ASSERT(listen_socket.Listen());

    // connect
    tcp::Socket socket;
    LSF_ASSERT(socket.Connect(tcp::SockAddr(ip::Address::Any(), listen_port)));

    // async accept
    EpollIOService io_service;
    LSF_ASSERT(io_service.AsyncAccept(listen_socket, AcceptFunc));

    io_service.Run();
}

int main(int argc, char **argv)
{
	LSF_TEST_ALL();
}



// vim:ts=4:sw=4:et:
