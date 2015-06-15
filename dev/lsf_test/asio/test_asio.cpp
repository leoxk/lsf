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

////////////////////////////////////////////////////////////
bool OnTimerFunc(AsyncInfo & info)
{
    static int counter= 0;
    counter++;

    if (counter >= 100)
    {
        IOService::Instance()->SetExit();
    }
    return true;
}

////////////////////////////////////////////////////////////
bool OnPeerCloseFunc(AsyncInfo & info, tcp::Socket client_socket)
{
    int fd = -1;
    LSF_ASSERT(IOService::Instance()->AsyncAddTimer(0, 1, OnTimerFunc, &fd));
    LSF_ASSERT(fd != -1);

    return true;
}

////////////////////////////////////////////////////////////
bool OnRecvFunc(AsyncInfo & info, tcp::Socket client_socket)
{
    // test msg
    LSF_ASSERT(info.buffer == content);

    // client close conn
    client_socket.Close();

    return true;
}

////////////////////////////////////////////////////////////
bool OnSendFunc(AsyncInfo & info, tcp::Socket server_socket)
{
    // test address
    tcp::Socket client_socket(info.fd);
    LSF_ASSERT(server_socket.RemoteSockAddr() == client_socket.LocalSockAddr());
    LSF_ASSERT(server_socket.LocalSockAddr() == client_socket.RemoteSockAddr());
    
    // read data
    LSF_ASSERT(server_socket.AsyncRead(*IOService::Instance(), 
                std::bind(OnRecvFunc, std::placeholders::_1, client_socket), 
                std::bind(OnPeerCloseFunc, std::placeholders::_1, client_socket)));

    // client shutdown
    client_socket.CloseAsync(*IOService::Instance());
    return true;
}

////////////////////////////////////////////////////////////
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
    LSF_ASSERT(client_socket.AsyncWrite(*IOService::Instance(), content.c_str(), content.size(), 
                std::bind(OnSendFunc, std::placeholders::_1, server_socket)));
    
    return true;
}

////////////////////////////////////////////////////////////
bool OnConnectFunc(AsyncInfo & info, tcp::ListenSocket listen_socket)
{
    // test listen address
    LSF_ASSERT(listen_socket.LocalSockAddr() == tcp::SockAddr(ip::Address::Any(), listen_port));

    // async accept
    tcp::Socket socket(info.fd);
    LSF_ASSERT(listen_socket.AsyncAccept(*IOService::Instance(), std::bind(OnAcceptFunc, std::placeholders::_1, socket)));

    return true;
}

LSF_TEST_CASE(test_asio)
{
    // listen
    tcp::ListenSocket listen_socket = tcp::ListenSocket::CreateListenSocket();
    LSF_ASSERT(listen_socket.Bind(tcp::SockAddr(ip::Address::Any(), listen_port)));
    LSF_ASSERT(listen_socket.Listen());

    // async connect
    tcp::Socket socket = tcp::Socket::CreateSocket();
    LSF_ASSERT(socket.AsyncConnect(*IOService::Instance(), tcp::SockAddr(ip::Address::Any(), listen_port),
                std::bind(OnConnectFunc, std::placeholders::_1, listen_socket)));

    IOService::Instance()->Run();
}

int main(int argc, char **argv)
{
	LSF_TEST_ALL(argc, argv);
}



// vim:ts=4:sw=4:et: