#pragma once

#include "svr/common/basic_http.h"
#include "svr/common/common_header.h"
#include "svr/common/common_proto.h"
#include "svr/3rd/json.hpp"

#include <google/protobuf/descriptor.h>
#include <google/protobuf/message.h>

class HttpService : public lsf::basic::Singleton<HttpService>{
    public:
        bool Init();
        void Exit();

        // void SendGetMatchListReply(const msg::PlatformGetMatchListRsp& response);
        // void SendQueryMatchReply(const msg::PlatformQueryMatchRsp& response);
        // void SendPutConfigReply(bool success = true, std::string msg = "");

    protected:
        static const size_t DEF_CHECK_INTERVAL = 10;
        static const size_t DEF_LOOP_TIME = 0;
        using Json = nlohmann::json;
        using ProtobufMsg = ::google::protobuf::Message;
        using ProtobufReflection = ::google::protobuf::Reflection;
        using ProtobufFieldDescriptor = ::google::protobuf::FieldDescriptor;
        using ProtobufDescriptor = ::google::protobuf::Descriptor;

    protected:
        // void GetMatchList(std::string query, std::string body);
        // void DelMatch(std::string query, std::string body);
        // void QueryMatch(std::string query, std::string body);
        // void GetConfig(std::string query, std::string body);
        // void PutConfig(std::string query, std::string body);

        void MatchSvrGmOperation(std::string query, std::string body);
        void GeneralSvrGmOperation(std::string query, std::string body);

        bool ParseJson(std::string uri, std::string const content);
        bool QueryStr2Message(std::string query, ProtobufMsg& message);
    private:
        BasicHttp _server;
        Json _json;
};
