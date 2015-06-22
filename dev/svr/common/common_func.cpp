// File:        common_func.cpp
// Description: ---
// Notes:       ---
// Author:      leoxiang <leoxiang727@qq.com>
// Revision:    2015-06-08 by leoxiang

#include "svr/common/common_func.h"
#include "svr/common/common_proto.h"
#include "svr/common/common_header.h"

using namespace lsf::basic;
using namespace lsf::util;

namespace common {

bool GetSingleMessageFromStream(std::string const &buffer, size_t &pos, std::string &message) {
    message.clear();
    if (pos == buffer.length()) return false;

    // clac begin and length
    char const *begin = buffer.data() + pos;
    size_t length = buffer.length() - pos;

    // check length
    if (length < sizeof(msg::Header)) {
        LSF_LOG_ERR("incomplete message, length=%u, %s", length, StringExt::BinToHexString(begin, length).c_str());
        return false;
    }

    // get header
    msg::Header header = *((msg::Header const *)begin);
    header.ntoh();

    // check header
    if (header.magic[0] != msg::MAGIC[0] || header.magic[1] != msg::MAGIC[1] || header.length <= sizeof(msg::Header)) {
        LSF_LOG_ERR("invalid header, magic=%x%x, length=%u", header.magic[0], header.magic[1], header.length);
        return false;
    }
    if (length < header.length) {
        LSF_LOG_ERR("incomplete message, buffer_length=%u, header_length=%u, %s", length, header.length,
                    StringExt::BinToHexString(begin, length).c_str());
        return false;
    }

    // set message
    message.assign(buffer, pos + sizeof(msg::Header), header.length - sizeof(msg::Header));

    // move pos
    pos += header.length;

    return true;
}
bool PutSingleMessageIntoStream(std::string &message) {
    // set header
    msg::Header header;
    header.magic[0] = msg::MAGIC[0];
    header.magic[1] = msg::MAGIC[1];
    header.length = sizeof(msg::Header) + message.length();
    header.hton();

    // insert header
    message.insert(0, (char const *)&header, sizeof(header));

    return true;
}

bool PackProtoMsg(std::string &message, google::protobuf::MessageLite const &proto_msg) {
    // pack protobuf
    if (!proto_msg.SerializeToString(&message)) {
        LSF_LOG_ERR("pack msg failed, %s", lsf::util::ProtobufLog::Instance()->ErrCharStr());
        LSF_LOG_STACK();
        return false;
    }

    return true;
}

bool UnPackProtoMsg(std::string const &message, google::protobuf::MessageLite &proto_msg) {
    // parse message
    if (!proto_msg.ParseFromString(message)) {
        LSF_LOG_ERR("pasre msg failed, %s", lsf::util::ProtobufLog::Instance()->ErrCharStr());
        LSF_LOG_STACK();
        return false;
    }

    return true;
}

bool SendMessage(lsf::asio::Socket socket, google::protobuf::MessageLite const &proto_msg) {
    std::string message;
    if (!PackProtoMsg(message, proto_msg)) return false;
    if (!PutSingleMessageIntoStream(message)) return false;

    if (!socket.Send(message)) {
        LSF_LOG_ERR("send msg failed, size=%u, %s", message.size(), socket.ErrCharStr());
        LSF_LOG_STACK();
        return false;
    }

    return true;
}

bool SendAndRecv(lsf::asio::Socket socket, google::protobuf::MessageLite &proto_msg) {
    // send request
    std::string message;
    if (!PackProtoMsg(message, proto_msg)) return false;
    if (!PutSingleMessageIntoStream(message)) return false;
    if (!socket.Send(message)) {
        LSF_LOG_ERR("send msg failed, size=%u, %s", message.size(), socket.ErrCharStr());
        return false;
    }

    // recv buffer
    std::string stream;
    if (!socket.Recv(stream)) {
        LSF_LOG_ERR("recv msg failed, %s", socket.ErrCharStr());
        return false;
    }

    // parse config
    size_t pos = 0;
    if (!GetSingleMessageFromStream(stream, pos, message)) return false;
    if (!UnPackProtoMsg(message, proto_msg)) return false;

    return true;
}

}  // end of namespace common

// vim:ts=4:sw=4:et:ft=cpp:
