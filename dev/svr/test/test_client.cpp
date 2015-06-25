// File:        test_client.cpp
// Description: ---
// Notes:       ---
// Author:      leoxiang <leoxiang727@qq.com>
// Revision:    2015-06-25 by leoxiang

#include <iostream>
#include "svr/common/common_header.h"

using namespace lsf::asio;
using namespace lsf::util;
using namespace lsf::basic;

bool ProcessLogin(Socket socket) {

    // if (socket
    return true;
}

int main(int argc, char **argv) {
    Socket socket = tcp::Socket::CreateSocket();
    SockAddr sockaddr = { "127.0.0.1|80100" };

    if (!socket.Connect(sockaddr)) {
        std::cout << LSF_DEBUG_INFO << std::endl;
    }

    return 0;
}

// vim:ts=4:sw=4:et:ft=cpp:
