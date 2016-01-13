// File:        common_func.cpp
// Description: ---
// Notes:       ---
// Author:      leoxiang <leoxiang727@qq.com>
// Revision:    2015-06-08 by leoxiang
#include <vector>
#include <map>
#include "svr/common/common_func.h"
#include "svr/common/common_header.h"
#include "svr/common/common_proto.h"
#include "svr/common/common_service.h"

using namespace lsf::basic;
using namespace lsf::util;
using namespace lsf::asio;
using namespace google::protobuf;

namespace common {

////////////////////////////////////////////////////////////
// stream to package
bool GetSingleMessageFromStream(std::string const& buffer, size_t& pos, std::string& content) {
    content.clear();
    if (pos == buffer.length()) return false;

    // clac begin and length
    char const* begin = buffer.data() + pos;
    size_t length = buffer.length() - pos;

    // check length
    if (length < sizeof(msg::Header)) return false;

    // get header
    msg::Header header = *((msg::Header const*)begin);
    header.ntoh();

    // check header
    if (length < header.length) return false;
    if (header.magic[0] != msg::MAGIC[0] || header.magic[1] != msg::MAGIC[1] || header.length <= sizeof(msg::Header)) {
        LSF_LOG_ERR("invalid header, magic=%x%x, length=%u, buffer_length=%u", header.magic[0], header.magic[1], header.length, length);
        return false;
    }

    // set content
    content.assign(buffer, pos + sizeof(msg::Header), header.length - sizeof(msg::Header));

    // move pos
    pos += header.length;

    return true;
}

bool PutSingleMessageIntoStream(std::string&  buffer, std::string const& content) {
    // set header
    msg::Header header;
    header.magic[0] = msg::MAGIC[0];
    header.magic[1] = msg::MAGIC[1];
    header.length = sizeof(msg::Header) + content.length();
    header.hton();

    // insert header
    buffer.clear();
    buffer.append((char const *)&header, sizeof(header));
    buffer.append(content);

    return true;
}

////////////////////////////////////////////////////////////
// send and recv
bool BlockSend(lsf::asio::SharedSocket socket, google::protobuf::Message& message) {
    // check input
    if (!socket->IsConnect()) {
        LSF_LOG_ERR_STACK("socket is not connected, %s, %u", LSF_ES(*socket), socket->SockFd());
        return false;
    }

    // send request
    std::string content;
    std::string buffer;
    if (!Protobuf::SerializeProtoMsgToString(content, message)) {
        LSF_LOG_ERR_STACK("serialize failed, %s", LSF_PROTOBUF);
        return false;
    }
    if (!PutSingleMessageIntoStream(buffer, content)) return false;
    if (!socket->Send(buffer)) {
        LSF_LOG_ERR_STACK("send msg failed, size=%u, %s, %u", buffer.size(), LSF_ES(*socket), socket->SockFd());
        return false;
    }

    return true;
}

bool BlockRecv(lsf::asio::SharedSocket socket, google::protobuf::Message& message) {
    // recv buffe
    std::string buffer;
    std::string content;
    size_t pos = 0;
    do {
        std::string tmp;
        if (!socket->Recv(tmp)) {
            LSF_LOG_ERR_STACK("recv msg failed, %s", LSF_ES(*socket));
            return false;
        }
        buffer.append(tmp);
        pos = 0;
    } while (!GetSingleMessageFromStream(buffer, pos, content));

    // if buffer has unused data, just save it
    if (pos < buffer.length()) {
        LSF_LOG_INF("there is some buffer not used, %u, %u", pos, buffer.length());
        IOService::Instance()->SaveUnusedData(socket, buffer.data() + pos, buffer.length() - pos);
    }

    // parse config
    if (!Protobuf::ParseProtoMsgFromString(content, message)) {
        LSF_LOG_ERR_STACK("parse failed, %s", LSF_PROTOBUF);
        return false;
    }

    return true;
}

bool SendAndRecv(lsf::asio::SharedSocket socket, google::protobuf::Message& message) {
    if (!BlockSend(socket, message)) return false;
    if (!BlockRecv(socket, message)) return false;
    return true;
}

////////////////////////////////////////////////////////////
// protobuf helper
void SetEnumIfHas(google::protobuf::Message& message, char const* name, int value) {
    // get descriptor and reflection
    auto const* pdesc = message.GetDescriptor();
    auto const* pref  = message.GetReflection();

    // get field descriptor
    auto const* pfdesc = pdesc->FindFieldByName(name);
    if (pfdesc == nullptr) return;
    if (pfdesc->type() != google::protobuf::FieldDescriptor::TYPE_ENUM) return;

    // get enum value descriptor
    auto const* pevdesc = pfdesc->enum_type()->FindValueByNumber(value);
    if (pevdesc == nullptr) return;

    // set enum value
    pref->SetEnum(&message, pfdesc, pevdesc);
}

void SetResponse(msg::CS const& request, msg::CS& response, msg::ENError error) {
    // get descriptor and reflection
    auto const* pdesc = msg::CS::descriptor();
    auto const* pref = request.GetReflection();

    for (int i = 0; i < pdesc->field_count(); ++i) {
        // get request field descriptor
        auto const* pfdesc = pdesc->field(i);
        if (!pref->HasField(request, pfdesc)) continue;
        if (i >= pdesc->field_count() - 1) continue;

        // get response field descriptor
        auto const* pfdesc_rsp = pdesc->field(i+1);
        auto* prsp = pref->MutableMessage(&response, pfdesc_rsp);

        // if has result, then assign
        common::SetEnumIfHas(*prsp, "result", error);
    }
}

void SetResponse(msg::SS const& request, msg::SS& response, msg::ENInternalError error) {
    // get descriptor and reflection
    auto const* pdesc = msg::SS::descriptor();
    auto const* pref = request.GetReflection();

    for (int i = 0; i < pdesc->field_count(); ++i) {
        // get request field descriptor
        auto const* pfdesc = pdesc->field(i);
        if (!pref->HasField(request, pfdesc)) continue;
        if (i >= pdesc->field_count() - 1) continue;

        // get response field descriptor
        auto const* pfdesc_rsp = pdesc->field(i+1);
        auto* prsp = pref->MutableMessage(&response, pfdesc_rsp);

        // if has result, then assign
        common::SetEnumIfHas(*prsp, "result", error);
    }
}

void ClearField(google::protobuf::Message& message, char const* name) {
    auto const* pdesc = message.GetDescriptor();
    auto const* pref  = message.GetReflection();
    auto const* pfdesc = pdesc->FindFieldByName(name);
    if (pfdesc == nullptr) return;
    if (!pref->HasField(message, pfdesc)) return;
    pref->ClearField(&message, pfdesc);
}

google::protobuf::Message* GenerateMessageFromName(char const* message_name) {
    // find descriptor
    Descriptor const* pdesc = DescriptorPool::generated_pool()->FindMessageTypeByName(message_name);
    if (pdesc == nullptr) return nullptr;

    // find message prototype
    Message const* prototype = MessageFactory::generated_factory()->GetPrototype(pdesc);
    if (prototype == nullptr) return nullptr;

    // return new instance
    return prototype->New();
}

bool HasKnownField(google::protobuf::Message const& message) {
    // get descriptor and reflection
    auto const* pdesc = message.GetDescriptor();
    auto const* pref  = message.GetReflection();

    for (int i = 0; i < pdesc->field_count(); ++i) {
        // get request field descriptor
        auto const* pfdesc = pdesc->field(i);
        if (!pref->HasField(message, pfdesc)) return false;
    }

    return true;
}

}  // end of namespace common

// vim:ts=4:sw=4:et:ft=cpp:
