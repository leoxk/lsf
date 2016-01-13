// File:        name_table_map.cpp
// Description: ---
// Notes:       ---
// Author:      leoxiang <leoxiang727@qq.com>
// Revision:    2015-08-14 by leoxiang

#include "svr/confsvrd/name_table_map.h"

using namespace lsf::basic;
using namespace lsf::util;
using namespace google::protobuf;

#define LSF_PARSE_ERR LSF_ERR << "[" << _fname << '|' << _line_count << "] "

static const char NAME_DELIMIT  = '.';
static const auto WHITE_SPACES = StringExt::WHITE_SPACES;

void NameTableMap::Clear() {
    _line_count = 0;
    _fs.close();
    _fname.clear();
    _line.clear();
    _block_name.clear();
    _message_map.clear();
     _enum_map.clear();
}

bool NameTableMap::Init(std::string const& file_name) {
    // open file
    _fs.open(file_name, std::ios::in);
    LSF_SCOPE_EXIT() { _fs.close(); };
    if (!_fs.is_open()) {
        LSF_ERR << "open file failed: " << file_name.c_str() << std::endl;
        return false;
    }

    // parse line
    _line_count = 0;
    while (std::getline(_fs, _line)) {
        // preprocess
        if (!PreProcessLine()) continue;

        // find package define
        if (IsStartWith(PACKAGE)) {
            if (!_block_name.empty()) _block_name.append(1, NAME_DELIMIT);
            static auto delimit = std::string(WHITE_SPACES) + "=;";
            _block_name.append(StringExt::SplitGet(_line, delimit, 1));
            continue;
        }

        // find message
        if (IsStartWith(MESSSAGE)) {
            if (!ProcessMessage()) return false;
            continue;
        }

        // find enum
        if (IsStartWith(ENUM)) {
            if (!ProcessEnum()) return false;
            continue;
        }
    }

    return true;
}

bool NameTableMap::TranslateMessageName(google::protobuf::Descriptor const* pdesc, std::string& ch_name, std::string& en_name) {
    // find name table
    auto iter = _message_map.find(pdesc->full_name());
    if (iter == _message_map.end()) {
        LSF_ERR << "cant find message in name table: " << pdesc->full_name() << std::endl;
        return false;
    }

    // get first ch name field
    std::string field = StringExt::SplitGet(ch_name, NAME_DELIMIT, 0);
    StringExt::RemoveHeadWhitespace(field);
    StringExt::RemoveTailWhitespace(field);
    ch_name.erase(0, field.size());
    if (!ch_name.empty()) ch_name.erase(0, 1);

    // find in name table
    auto field_iter = iter->second.find(field);
    if (field_iter == iter->second.end()) {
        LSF_ERR << "cant find field in name table: " << "[" << field << "]" << std::endl;
        // for (auto it : iter->second) LSF_ERR << it.first << " [" << it.second << "]" << std::endl;
        return false;
    }
    auto const* pfdesc = pdesc->FindFieldByName(field_iter->second);
    if (pfdesc == nullptr) {
        LSF_ERR << "cant find field in protobuf meta: " << pdesc->full_name() << " " << field_iter->second << std::endl;
        return false;
    }

    // append en name
    en_name.append(en_name.empty() ? pfdesc->name() : NAME_DELIMIT + pfdesc->name());

    // if is repeated
    if (pfdesc->label() == FieldDescriptor::LABEL_REPEATED) {
        std::string next_field = StringExt::SplitGet(ch_name, NAME_DELIMIT, 0);
        en_name.append(NAME_DELIMIT + next_field);
        ch_name.erase(0, next_field.size());
        if (!ch_name.empty()) ch_name.erase(0, 1);
    }

    // if is message type
    if (pfdesc->type() == FieldDescriptor::TYPE_MESSAGE) {
        // recursive call
        return TranslateMessageName(pfdesc->message_type(), ch_name, en_name);
    }
    // if is non-message type
    else {
        // ch name must be empty
        if (!ch_name.empty()) {
            LSF_ERR << "message is non-message type but ch name not empty: " << pfdesc->full_name() << " " << ch_name << std::endl;
            return false;
        }
        return true;
    }
}

bool NameTableMap::TranslateEnumName(std::string const& enum_name, std::string& value_name) {
    // find enum
    auto iter = _enum_map.find(enum_name);
    if (iter == _enum_map.end()) {
        LSF_ERR << "cant find enum: " << enum_name << std::endl;
        return false;
    }

    // find ch_name
    auto enum_iter = iter->second.find(value_name);
    if (enum_iter == iter->second.end()) {
        LSF_ERR << "cant find enum ch name: " << enum_name << " " << value_name << std::endl;
        return false;
    }

    // found
    value_name = enum_iter->second;
    return true;
}

bool NameTableMap::ProcessMessage() {
    // process block name
    if (!ProcessBlockName()) return false;

    // process until block end
    while (std::getline(_fs, _line)) {
        // preprocess
        if (!PreProcessLine()) continue;

        // find nest message
        if (IsStartWith(MESSSAGE)) {
            if (!ProcessMessage()) return false;
            continue;
        }

        // find nest enum
        if (IsStartWith(ENUM)) {
            if (!ProcessEnum()) return false;
            continue;
        }

        // get block end
        if (IsStartWith(BLOCK_END)) {
            RemoveBlockName();
            return true;
        }

        // find message item
        auto ch_name = GetTailComment();
        if (ch_name.empty()) continue;
        auto& name_table = _message_map[_block_name];
        if (name_table.find(ch_name) != name_table.end()) {
            LSF_PARSE_ERR << "duplicate ch name: " << ch_name << std::endl;
            return false;
        }

        // assign value
        static auto delimit = std::string(WHITE_SPACES) + "=;";
        name_table[ch_name] = StringExt::SplitGet(_line, delimit, 2, true);
    }

    LSF_PARSE_ERR << "cant find block end, " << _block_name << std::endl;
    return false;
}

bool NameTableMap::ProcessEnum() {
    // process block name
    if (!ProcessBlockName()) return false;

    // process until block end
    while (std::getline(_fs, _line)) {
        // preprocess
        if (!PreProcessLine()) continue;

        // find nest message
        if (IsStartWith(MESSSAGE)) {
            LSF_PARSE_ERR << "not support nest message in enum" << std::endl;
            return false;
        }

        // find nest enum
        if (IsStartWith(ENUM)) {
            LSF_PARSE_ERR << "not support nest enum in enum" << std::endl;
            return false;
        }

        // get block end
        if (IsStartWith(BLOCK_END)) {
            RemoveBlockName();
            return true;
        }

        // find enum item
        auto ch_name = GetTailComment();
        if (ch_name.empty()) continue;
        auto& name_table = _enum_map[_block_name];
        if (name_table.find(ch_name) != name_table.end()) {
            LSF_PARSE_ERR << "duplicate ch name: " << ch_name << std::endl;
            return false;
        }

        // assign value
        static auto delimit = std::string(WHITE_SPACES) + "=;";
        name_table[ch_name] = StringExt::SplitGet(_line, delimit, 0, true);
    }

    LSF_PARSE_ERR << "cant find block end, " << _block_name << std::endl;
    return false;
}

bool NameTableMap::PreProcessLine() {
    // add count
    _line_count++;

    // trim white spaces
    StringExt::RemoveHeadWhitespace(_line);
    StringExt::RemoveTailWhitespace(_line);

    // ignore comment
    if (_line.substr(0, 2) == COMMENT) return false;

    // ignore option
    if (IsStartWith(OPTION)) return false;

    // remove square braclet
    StringExt::FindAndTrimBlock(_line, '[', ']');

    return true;
}

bool NameTableMap::ProcessBlockName() {
    // get block name
    std::string msg_name;
    static auto delimit = std::string(WHITE_SPACES) + "{";
    msg_name = StringExt::SplitGet(_line, delimit, 1);
    if (msg_name.empty()) {
        LSF_PARSE_ERR << "get block without name" << std::endl;
        return false;
    }

    // set block name
    if (!_block_name.empty()) _block_name.append(1, NAME_DELIMIT);
    _block_name.append(msg_name);
    return true;
}

void NameTableMap::RemoveBlockName() {
    // trim tailing name
    size_t find = _block_name.rfind(NAME_DELIMIT);
    if (find == std::string::npos) _block_name.clear();
    else                           _block_name.erase(find);
}

std::string NameTableMap::GetTailComment() {
    // find comment begin
    size_t comment_begin = _line.find(COMMENT);
    if (comment_begin == std::string::npos) return "";

    // find content begin
    static auto delimit = std::string(WHITE_SPACES) + "/";
    size_t content_begin = _line.find_first_not_of(delimit, comment_begin);
    if (content_begin == std::string::npos) return "";

    // find content end
    size_t content_end = _line.find_first_of(delimit, content_begin);
    return _line.substr(content_begin, content_end == std::string::npos ? content_end : content_end - content_begin);
}

bool NameTableMap::IsStartWith(char const* token) {
    static auto delimit = std::string(WHITE_SPACES) + "{";
    return StringExt::SplitGet(_line, delimit, 0, true) == token;
}

// vim:ts=4:sw=4:et:ft=cpp:
