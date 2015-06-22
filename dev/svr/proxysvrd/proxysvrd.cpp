// File:        proxysvrd.cpp
// Description: ---
// Notes:       ---
// Author:      leoxiang <leoxiang727@qq.com>
// Revision:    2015-06-10 by leoxiang

#include "svr/common/common_header.h"
#include "svr/proxysvrd/proxysvrd.h"

using namespace google::protobuf;
using namespace lsf::util;

bool ProxyServer::OnRun() { return true; }

bool ProxyServer::OnClientMessage() { return true; }

int main(int argc, char** argv) { ProxyServer::Instance()->Run(argc, argv); }

// vim:ts=4:sw=4:et:ft=cpp:
