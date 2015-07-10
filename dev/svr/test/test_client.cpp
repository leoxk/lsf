// File:        test_client.cpp
// Description: ---
// Notes:       ---
// Author:      leoxiang <leoxiang727@qq.com>
// Revision:    2015-06-25 by leoxiang

#include <iostream>
#include "svr/common/common_header.h"
#include "svr/common/common_proto.h"
#include "svr/common/common_func.h"

using namespace lsf::asio;
using namespace lsf::util;
using namespace lsf::basic;

#define ROOM_ADDRESS "127.0.0.1|60402"

bool ProcessQuickStart(msg::CSHead const& cs_head) {
    // connect
    Socket socket = tcp::Socket::CreateSocket();
    SockAddr sockaddr = { ROOM_ADDRESS };
    if (!socket.Connect(sockaddr)) std::cout << LSF_DEBUG_INFO << std::endl;

    // request
    msg::CS message;
    message.mutable_cs_head()->CopyFrom(cs_head);
    message.set_msg_type(msg::CS_TYPE_QUICK_START_REQ);
    message.mutable_quick_start_req();

    // send and recv
    common::SendAndRecv(socket, message);

    return true;
}


int main(int argc, char **argv) {
    // common cs head
    msg::CSHead cs_head;
    cs_head.set_uid(0);
    cs_head.set_platform(msg::PLATFORM_TYPE_WEB);
    cs_head.set_verify_sig("");
    cs_head.set_client_version(0);

    // quick start
    if (!ProcessQuickStart(cs_head)) return 1;

    return 0;
}

// vim:ts=4:sw=4:et:ft=cpp:
