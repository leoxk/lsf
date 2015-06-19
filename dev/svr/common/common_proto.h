// File:        common_proto.h
// Description: ---
// Notes:       ---
// Author:      leoxiang <leoxiang727@qq.com>
// Revision:    2015-06-17 by leoxiang

#pragma once

#include "svr/proto/msg_base.pb.h"
#include "svr/proto/msg_cs.pb.h"
#include "svr/proto/msg_ss.pb.h"

namespace msg {

static const uint8_t MAGIC[] = { 0xf8, 0x8f };

struct Header
{
    void ntoh() { length = ntohl(length); }
    void hton() { length = htonl(length); }

    uint8_t  magic[2];
    uint32_t length;
};

} // end of namespace msg

// vim:ts=4:sw=4:et:ft=cpp:
