// File:        common_func.cpp
// Description: ---
// Notes:       ---
// Author:      leoxiang <leoxiang727@qq.com>
// Revision:    2015-06-08 by leoxiang

#include "svr/common/common_func.h"


bool CommonFunc::PackMsg(std::string & content, google::protobuf::MessageLite const & message)
{
    if (!message.SerializeToString(&content))
    {
        LSF_LOG_ERR("%s", lsf::util::ProtobufLog::Instance()->ErrCharStr());
        LSF_LOG_STACK();
        return false;
    }
    return true;
}

bool CommonFunc::UnPackMsg(std::string const & content, google::protobuf::MessageLite & message)
{
    if (!message.ParseFromString(content))
    {
        LSF_LOG_ERR("size=%u, %s", content.size(), lsf::util::ProtobufLog::Instance()->ErrCharStr());
        LSF_LOG_STACK();
        return false;
    }
    return true;
}

bool CommonFunc::SendAll(lsf::asio::tcp::Socket socket, std::string const & content)
{
    if (!socket.SendAll(content))
    {
        LSF_LOG_ERR("size=%u, %s", content.size(), socket.ErrCharStr());
        LSF_LOG_STACK();
        return false;
    }
    return true;
}

bool CommonFunc::RecvAll(lsf::asio::tcp::Socket socket, std::string & content)
{
    if (!socket.RecvAll(content))
    {
        LSF_LOG_ERR("%s", socket.ErrCharStr());
        LSF_LOG_STACK();
        return false;
    }
    return true;
}

// vim:ts=4:sw=4:et:ft=cpp:
