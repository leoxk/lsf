// File:        name_table_map.h
// Description: ---
// Notes:       ---
// Author:      leoxiang <leoxiang727@qq.com>
// Revision:    2015-08-14 by leoxiang

#include "svr/common/common_proto.h"
#include "svr/common/common_header.h"
#include "svr/common/common_func.h"

////////////////////////////////////////////////////////////
// NameTableMap
class NameTableMap : public lsf::basic::Error, public lsf::basic::Singleton<NameTableMap> {
public:
    constexpr static char const* COMMENT   = "//";
    constexpr static char const* MESSSAGE  = "message";
    constexpr static char const* ENUM      = "enum";
    constexpr static char const* BLOCK_END = "}";
    constexpr static char const* PACKAGE   = "package";
    constexpr static char const* OPTION    = "option";
    constexpr static char const* OPTIONAL  = "optional";
    constexpr static char const* REQUIRED  = "required";
    constexpr static char const* REPEATED  = "repeated";
    using message_type     = std::map<std::string,std::string>;
    using enum_type        = std::map<std::string,std::string>;
    using message_map_type = std::map<std::string, message_type>;
    using enum_map_type    = std::map<std::string, enum_type>;

public:
    bool Init(std::string const& file_name);
    void Clear();
    bool TranslateMessageName(google::protobuf::Descriptor const* pdesc, std::string& ch_name, std::string& en_name);
    bool TranslateEnumName(std::string const& enum_name, std::string& value_name);

private:
    bool ProcessMessage();
    bool ProcessEnum();
    bool PreProcessLine();
    bool ProcessBlockName();
    void RemoveBlockName();
    std::string GetTailComment();
    bool IsStartWith(char const* token);

private:
    size_t _line_count;
    std::fstream _fs;
    std::string _fname;
    std::string _line;
    std::string _block_name;
    message_map_type _message_map;
    enum_map_type _enum_map;
};

// vim:ts=4:sw=4:et:ft=cpp:
