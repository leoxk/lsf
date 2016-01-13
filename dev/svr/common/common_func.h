// File:        common_func.h
// Description: ---
// Notes:       ---
// Author:      leoxiang <leoxiang727@qq.com>
// Revision:    2015-06-08 by leoxiang

#pragma once
#include "svr/common/common_header.h"
#include "svr/common/common_proto.h"
#include <string>
#include <set>
#include "svr/proto/msg_cs.pb.h"
using namespace std;
namespace common {

////////////////////////////////////////////////////////////
// stream to package
bool GetSingleMessageFromStream(std::string const& buffer, size_t& pos, std::string& content);
bool PutSingleMessageIntoStream(std::string & buffer, std::string const& content);

////////////////////////////////////////////////////////////
// send and recv
bool SendAndRecv(lsf::asio::SharedSocket socket, google::protobuf::Message& message);
bool BlockSend(lsf::asio::SharedSocket socket, google::protobuf::Message& message);
bool BlockRecv(lsf::asio::SharedSocket socket, google::protobuf::Message& message);

////////////////////////////////////////////////////////////
// protobuf helper
void SetEnumIfHas(google::protobuf::Message& message, char const* name, int value);
void SetResponse(msg::CS const& request, msg::CS& response, msg::ENError error);
void SetResponse(msg::SS const& request, msg::SS& response, msg::ENInternalError error);
void ClearField(google::protobuf::Message& message, char const* name);
google::protobuf::Message* GenerateMessageFromName(char const* message_name);
bool HasKnownField(google::protobuf::Message const& message);

////////////////////////////////////////////////////////////
// template func
template<typename MsgType>
MsgType* GetSubMsg(google::protobuf::Message& message, char const* name) {
    auto const* pdesc = message.GetDescriptor();
    auto const* pref  = message.GetReflection();
    auto const* pfdesc = pdesc->FindFieldByName(name);
    if (pfdesc == nullptr) return nullptr;
    if (pfdesc->type() != google::protobuf::FieldDescriptor::TYPE_MESSAGE) return nullptr;
    if (!pref->HasField(message, pfdesc)) return nullptr;
    if (MsgType::descriptor() != pfdesc->message_type()) return nullptr;

    return static_cast<MsgType*>(pref->MutableMessage(&message, pfdesc));
}

template<typename MsgType>
MsgType const* GetSubMsg(google::protobuf::Message const& message, char const* name) {
    auto const* pdesc = message.GetDescriptor();
    auto const* pref  = message.GetReflection();
    auto const* pfdesc = pdesc->FindFieldByName(name);
    if (pfdesc == nullptr) return nullptr;
    if (pfdesc->type() != google::protobuf::FieldDescriptor::TYPE_MESSAGE) return nullptr;
    if (!pref->HasField(message, pfdesc)) return nullptr;
    if (MsgType::descriptor() != pfdesc->message_type()) return nullptr;

    return static_cast<MsgType const*>(&pref->GetMessage(message, pfdesc));
}

template<typename Type1, typename Type2>
void CopyMsg(google::protobuf::RepeatedPtrField<Type1>& list1, google::protobuf::RepeatedPtrField<Type2> const& list2) {
    for (auto const& item : list2) CopyMsg(*list1.Add(), item);
}

}  // end of namespace common

// vim:ts=4:sw=4:et:ft=cpp:
