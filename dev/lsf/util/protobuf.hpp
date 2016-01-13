// File:        protobuf.hpp
// Description: ---
// Notes:       ---
// Author:      leoxiang <leoxiang727@qq.com>
// Revision:    2014-09-30 by leoxiang

#pragma once
#include <vector>
#include <fstream>
#include <google/protobuf/stubs/common.h>
#include <google/protobuf/message.h>
#include <google/protobuf/text_format.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>
#include "lsf/basic/error.hpp"
#include "lsf/basic/singleton.hpp"
#include "lsf/basic/type_cast.hpp"

namespace lsf {
namespace util {

////////////////////////////////////////////////////////////
// Protobuf
class Protobuf : public lsf::basic::Singleton<Protobuf>, public lsf::basic::Error {
public:
    ////////////////////////////////////////////////////////////
    // log handler
    static void ProtoBufLogHandler(google::protobuf::LogLevel level, char const* filename, int line, std::string const& message) {
        Protobuf::Instance()->LogHandle(level, filename, line, message);
    }

    void Init() { google::protobuf::SetLogHandler(ProtoBufLogHandler); }

    void LogHandle(google::protobuf::LogLevel level, char const* filename, int line, std::string const& message) {
        SetErrString("[[" + std::string(filename) + ":" + lsf::basic::TypeCast<std::string>(line) + "|" + message + "]]");
    }

public:
    ////////////////////////////////////////////////////////////
    // message <--> binary string
    static inline bool SerializeProtoMsgToString(std::string& content, google::protobuf::Message const& message) {
        return message.SerializeToString(&content);
    }

    static inline bool ParseProtoMsgFromString(std::string const& content, google::protobuf::Message& message) {
        return message.ParseFromString(content);
    }

    ////////////////////////////////////////////////////////////
    // message <--> text string
    static inline bool SerializeProtoMsgToTextFormatString(std::string& content, google::protobuf::Message const& message) {
        return google::protobuf::TextFormat::PrintToString(message, &content);
    }

    static inline bool ParseProtoMsgFromTextFormatString(std::string const& content, google::protobuf::Message& message) {
        return google::protobuf::TextFormat::ParseFromString(content, &message);
    }

    static inline std::string MsgToTextFormatString(google::protobuf::Message const& message) {
        std::string content;
        SerializeProtoMsgToTextFormatString(content, message);
        return content;
    }

    ////////////////////////////////////////////////////////////
    // message <--> bin file
    static inline bool SerializeProtoMsgToBinFile(std::string const& file, google::protobuf::Message const& message) {
        // open file
        std::ofstream ofs(file, std::ios::out | std::ios::binary | std::ios::trunc);
        if (!ofs) return false;

        // serialize protobuf
        if (!message.SerializeToOstream(&ofs)) return false;

        return true;
    }

    static inline bool ParseProtoMsgFromBinFile(std::string const& file, google::protobuf::Message& message) {
        // open file
        std::ifstream ifs(file, std::ios::in | std::ios::binary);
        if (!ifs) return false;

        // parse protobuf
        if (!message.ParseFromIstream(&ifs)) return false;

        return true;
    }

    ////////////////////////////////////////////////////////////
    // message <--> text file
    static inline bool SerializeProtoMsgToFile(std::string const& file, google::protobuf::Message const& message) {
        // open file
        std::ofstream ofs(file, std::ios::out | std::ios::trunc);
        if (!ofs) return false;

        // serialize protobuf
        google::protobuf::io::OstreamOutputStream osos(&ofs);
        if (!google::protobuf::TextFormat::Print(message, &osos)) return false;

        return true;
    }

    static inline bool ParseProtoMsgFromFile(std::string const& file, google::protobuf::Message& message) {
        // open file
        std::ifstream ifs(file, std::ios::in);
        if (!ifs) return false;

        // parse protobuf
        google::protobuf::io::IstreamInputStream isis(&ifs);
        if (!google::protobuf::TextFormat::Parse(&isis, &message)) return false;

        return true;
    }

    ////////////////////////////////////////////////////////////
    // message <--> json
    static bool SerializeProtoMsgToJson(std::string& content, google::protobuf::Message const& message) {
        // // get descriptor and reflection
        // auto* pref = message.GetReflection();
        // auto* pdesc = message.GetDescriptor();
        //
        // // heading braclet
        // std::ostringstream oss;
        // oss << "{";
        //
        // // travers all field
        // for (int i = 0; i < pdesc->field_count(); ++i) {
        // }
        return true;
    }
};

}  // end of namespace util
}  // end of namespace lsf

// vim:ts=4:sw=4:et:
