// File:        common_proto.h
// Description: ---
// Notes:       ---
// Author:      leoxiang <leoxiang727@qq.com>
// Revision:    2015-06-17 by leoxiang

#pragma once
#include <arpa/inet.h>
#include "lsf/basic/macro.hpp"
#include "svr/proto/conf_deploy.pb.h"
#include "svr/proto/conf_game.pb.h"
#include "svr/proto/msg_cs.pb.h"
#include "svr/proto/data_mem.pb.h"
#include "svr/proto/data_db.pb.h"
#include "svr/proto/msg_ss.pb.h"

////////////////////////////////////////////////////////////
// tcp stream header
namespace msg {

#pragma pack(push)
#pragma pack(1)
class Header {
public:
    void ntoh() { length = ntohl(length); }
    void hton() { length = htonl(length); }
    uint32_t length;
};
#pragma pack(pop)

}  // end of namespace msg

////////////////////////////////////////////////////////////
// enum to string
LSF_ENUM_TO_STRING_PROTO(msg::ENPlatformType)
LSF_ENUM_TO_STRING_PROTO(msg::ENCSType)
LSF_ENUM_TO_STRING_PROTO(msg::ENSSType)
LSF_ENUM_TO_STRING_PROTO(msg::ENError)
LSF_ENUM_TO_STRING_PROTO(msg::ENInternalError)
LSF_ENUM_TO_STRING_PROTO(data::ENTimerType)
LSF_ENUM_TO_STRING_PROTO(data::ENDataType)
LSF_ENUM_TO_STRING_PROTO(conf::ENServerType)
LSF_ENUM_TO_STRING_PROTO(conf::ENServiceType)

// vim:ts=4:sw=4:et:ft=cpp:
