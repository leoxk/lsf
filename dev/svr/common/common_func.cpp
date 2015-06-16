// File:        common_func.cpp
// Description: ---
// Notes:       ---
// Author:      leoxiang <leoxiang727@qq.com>
// Revision:    2015-06-08 by leoxiang

#include "svr/common/common_func.h"

namespace common {

bool PackMsg(std::string & content, google::protobuf::MessageLite const & message)
{
    if (!message.SerializeToString(&content))
    {
        LSF_LOG_ERR("pack msg failed, %s", lsf::util::ProtobufLog::Instance()->ErrCharStr());
        LSF_LOG_STACK();
        return false;
    }
    return true;
}

bool UnPackMsg(std::string const & content, google::protobuf::MessageLite & message)
{
    if (!message.ParseFromString(content))
    {
        LSF_LOG_ERR("unpack msg failed, size=%u, %s", content.size(), lsf::util::ProtobufLog::Instance()->ErrCharStr());
        LSF_LOG_STACK();
        return false;
    }
    return true;
}

bool SendAll(lsf::asio::tcp::Socket socket, std::string const & content)
{
    if (!socket.SendAll(content))
    {
        LSF_LOG_ERR("send msg failed, size=%u, %s", content.size(), socket.ErrCharStr());
        LSF_LOG_STACK();
        return false;
    }
    return true;
}

bool RecvAll(lsf::asio::tcp::Socket socket, std::string & content)
{
    if (!socket.RecvAll(content))
    {
        LSF_LOG_ERR("recv msg failed, %s", socket.ErrCharStr());
        LSF_LOG_STACK();
        return false;
    }
    return true;
}

} // end of namespace common

// vim:ts=4:sw=4:et:ft=cpp:
