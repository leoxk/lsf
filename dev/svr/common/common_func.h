// File:        common_func.h
// Description: ---
// Notes:       ---
// Author:      leoxiang <leoxiang727@qq.com>
// Revision:    2015-06-08 by leoxiang

#pragma once

#include <string>
#include "lsf/util/protobuf_log.hpp"
#include "lsf/util/backtrace.hpp"
#include "lsf/util/log.hpp"
#include "lsf/asio/tcp.hpp"
#include "lsf/asio/net.hpp"
#include "svr/proto/conf_deploy.pb.h"

namespace common {

////////////////////////////////////////////////////////////
// pack and unpack
bool GetSingleMessageFromStream(std::string const& buffer, size_t& pos, std::string& message);
bool PutSingleMessageIntoStream(std::string& message);

bool PackProtoMsg(std::string& message, google::protobuf::MessageLite const& proto_msg);
bool UnPackProtoMsg(std::string const& message, google::protobuf::MessageLite& proto_msg);

////////////////////////////////////////////////////////////
// send and recv
bool SendMessage(lsf::asio::Socket socket, google::protobuf::MessageLite const& message);
bool SendAndRecv(lsf::asio::Socket socket, google::protobuf::MessageLite& message);

}  // end of namespace common

// vim:ts=4:sw=4:et:ft=cpp:
