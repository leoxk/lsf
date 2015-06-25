// File:        game_server.cpp
// Description: ---
// Notes:       ---
// Author:      leoxiang <leoxiang727@qq.com>
// Revision:    2015-06-25 by leoxiang

#include "svr/common/common_header.h"
#include "svr/common/common_service.h"
#include "svr/gamesvrd/game_server.h"

using namespace google::protobuf;
using namespace lsf::util;

bool GameServer::OnRun() {
    // ConnectPro
    return true;
}

int main(int argc, char** argv) { GameServer::Instance()->Run(argc, argv); }

// vim:ts=4:sw=4:et:ft=cpp:
