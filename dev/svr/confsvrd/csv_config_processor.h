// File:        csv_config_processor.h
// Description: ---
// Notes:       ---
// Author:      leoxiang <leoxiang727@qq.com>
// Revision:    2015-08-14 by leoxiang

#include "svr/common/common_proto.h"
#include "svr/common/common_header.h"
#include "svr/common/common_func.h"

////////////////////////////////////////////////////////////
// Main Process Routine
class CSVConfigProcessor : public lsf::basic::Singleton<CSVConfigProcessor>, public lsf::basic::Error {
public:
    bool Process2DTable(google::protobuf::RepeatedPtrField<conf::Integer2DTable>& table, std::string const& file_name);
    bool ProcessConfig(google::protobuf::Message& message, std::string const& file_name, std::string const& msg_name);

private:
    bool ProcessField(google::protobuf::Message& message);
    bool ProcessSetField(google::protobuf::Message& message);
    bool ProcessSetRepeatedField(google::protobuf::Message& message);
    bool ProcessAddRepeatedField(google::protobuf::Message& message);
    std::string GetFirstToken();
    bool StringToBool(std::string const& field);

private:
    google::protobuf::Descriptor const*      _pdesc;
    google::protobuf::Reflection const*      _pref;
    google::protobuf::FieldDescriptor const* _pfdesc;
    std::string _name;
    std::string _field;
    std::string _repeated_mark;
    std::vector<std::string> _name_table;
    size_t _line_count;
};


// vim:ts=4:sw=4:et:ft=cpp:
