// File:        connsvrd.cpp
// Description: ---
// Notes:       ---
// Author:      leoxiang <leoxiang727@qq.com>
// Revision:    2015-06-22 by leoxiang

#include "svr/common/common_header.h"
#include "svr/connsvrd/connsvrd.h"

using namespace google::protobuf;
using namespace lsf::util;

bool ConnectServer::OnRun() { return true; }

bool ConnectServer::OnClientMessage() { return true; }

int main(int argc, char** argv) { ConnectServer::Instance()->Run(argc, argv); }

// vim:ts=4:sw=4:et:ft=cpp:
