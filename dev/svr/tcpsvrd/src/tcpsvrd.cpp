// File:        tcpsvrd.cpp
// Description: ---
// Notes:       ---
// Author:      leoxiang <leoxiang727@qq.com>
// Revision:    2015-06-08 by leoxiang

#include "tcpsvrd.h"

int main(int argc, char** argv)
{
    TcpServer server;
    server.UseEpoll();
    server.Run(argc, argv);
}

// vim:ts=4:sw=4:et:ft=cpp:
