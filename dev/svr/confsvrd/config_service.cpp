// File:        config_service.cpp
// Description: ---
// Notes:       ---
// Author:      leoxiang <leoxiang727@qq.com>
// Revision:    2015-06-15 by leoxiang

#include "svr/common/common_header.h"
#include "svr/common/common_func.h"
#include "svr/confsvrd/config_service.h"
#include "svr/proto/msg_ss.pb.h"

bool ConfigService::OnSocketRead(lsf::asio::AsyncInfo & info)
{
    msg::SS message;
    if (!CommonFunc::UnPackMsg(info.buffer, message)) return true;

    if (message.type() != msg::SS_GET_CONFIG_REQ) ;

    return true;
}

// vim:ts=4:sw=4:et:ft=cpp:
