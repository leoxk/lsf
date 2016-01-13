// File:        csv_config_processor.cpp
// Description: ---
// Notes:       ---
// Author:      leoxiang <leoxiang727@qq.com>
// Revision:    2015-08-14 by leoxiang

#include "lsf/util/locale.hpp"
#include "lsf/algorithm/two_dimensional_table.hpp"
#include "svr/confsvrd/csv_config_processor.h"
#include "svr/confsvrd/name_table_map.h"

using namespace lsf::algorithm;
using namespace lsf::basic;
using namespace lsf::util;
using namespace google::protobuf;

static const char NAME_DELIMIT  = '.';
static const char FIELD_DELIMIT = ',';

bool CSVConfigProcessor::Process2DTable(google::protobuf::RepeatedPtrField<conf::Integer2DTable>& table, std::string const& file_name) {
    // error output
    LSF_SCOPE_EXIT_NAME(error) { LSF_LOG_ERR("failed process %s", file_name.c_str()); };

    // parse table
    TwoDimensionalTable<int> int_table;
    if (!int_table.ParseFromFile(file_name)) return false;

    // assign value
    for (auto& table_line : int_table) {
        auto* pmsg = table.Add();
        for (auto& field : table_line) pmsg->add_fields(field);
    }

    LSF_LOG_INF("success process %s, config_size=%u", file_name.c_str(), table.size());
    error.Dismiss();
    return true;
}

bool CSVConfigProcessor::ProcessConfig(google::protobuf::Message& message, std::string const& file_name, std::string const& msg_name) {
    // error output
    LSF_SCOPE_EXIT_NAME(error) { LSF_LOG_ERR("failed process %s", file_name.c_str()); };

    // find right desc
    FieldDescriptor const* pfdesc = nullptr;
    for (int i = 0; i < message.GetDescriptor()->field_count(); ++i) {
        auto const* p_tmp_desc = message.GetDescriptor()->field(i);
        if (p_tmp_desc->message_type()->full_name() == msg_name && p_tmp_desc->label() == FieldDescriptor::LABEL_REPEATED) {
            pfdesc = p_tmp_desc;
            break;
        }
    }
    if (pfdesc == nullptr) {
        SetErrString("cant find message descriptor: " + msg_name);
        LSF_ERR << ErrString() << std::endl;
        return false;
    }

    // open file
    TwoDimensionalTable<std::string> table;
    if (!table.ParseFromFile(file_name)) {
        SetErrString("open file failed: "  + file_name);
        LSF_ERR << ErrString() << std::endl;
        return false;
    }

    // init name table
    std::vector<std::string> name_table;
    auto* pdesc = DescriptorPool::generated_pool()->FindMessageTypeByName(msg_name);
    if (pdesc == nullptr) {
        SetErrString("cant find message from pool: " + msg_name);
        LSF_ERR << ErrString() << std::endl;
        return false;
    }
    auto& table_line = table.at(0);
    for (auto& ch_name : table_line) {
        std::string en_name;
        auto ret = NameTableMap::Instance()->TranslateMessageName(pdesc, ch_name, en_name);
        name_table.push_back(ret ? en_name : "");
    }

    // process other line
    for (size_t i = 1; i < table.size(); ++i) {
        // add new message
        Message* pmesg = message.GetReflection()->AddMessage(&message, pfdesc);
        if (pmesg == nullptr) {
            SetErrString("cant add new message: " + msg_name);
            LSF_ERR << ErrString() << std::endl;
            return false;
        }

        // clear mark
        _repeated_mark.clear();

        // traverse every field
        for (size_t j = 0; j < name_table.size(); ++j) {
            _name = name_table[j];
            _field = table.Get(i, j);

            // ignore empty token
            if (_name.empty()) continue;
            if (_field.empty()) continue;
            if (!ProcessField(*pmesg)) return false;
        }
    }

    LSF_LOG_INF("success process %s, config_size=%u", file_name.c_str(), message.GetReflection()->FieldSize(message, pfdesc));
    error.Dismiss();
    return true;
}

bool CSVConfigProcessor::ProcessField(Message& message) {
    // descriptor and reflection
    _pdesc = message.GetDescriptor();
    _pref = message.GetReflection();

    // get first name
    auto field_name = GetFirstToken();

    // get field desc
    _pfdesc = _pdesc->FindFieldByName(field_name);
    if (_pfdesc == nullptr) {
        SetErrString("cant find field: " + _pdesc->full_name() + " " + _name + " " + field_name);
        LSF_ERR << ErrString() << std::endl;
        return false;
    }

    // if is repeated
    if (_pfdesc->label() == FieldDescriptor::LABEL_REPEATED) {
        auto field_size = _pref->FieldSize(message, _pfdesc);
        auto mark = GetFirstToken();
        // if same, get last one
        if (mark == _repeated_mark && field_size != 0) {
            ProcessSetRepeatedField(message);
        }
        // not same, create new one
        else {
            _repeated_mark = mark;
            ProcessAddRepeatedField(message);
        }
    }
    // process single field
    else {
        ProcessSetField(message);
    }

    return true;
}

bool CSVConfigProcessor::ProcessSetField(Message& message) {
    // get size
    switch (_pfdesc->type()) {
        case FieldDescriptor::TYPE_DOUBLE:   _pref->SetDouble(&message, _pfdesc, TypeCast<double>(_field)); return true;
        case FieldDescriptor::TYPE_FLOAT:    _pref->SetFloat(&message, _pfdesc, TypeCast<float>(_field)); return true;
        case FieldDescriptor::TYPE_SFIXED64:
        case FieldDescriptor::TYPE_SINT64:
        case FieldDescriptor::TYPE_INT64:    _pref->SetInt64(&message, _pfdesc, TypeCast<int64_t>(_field)); return true;
        case FieldDescriptor::TYPE_SFIXED32:
        case FieldDescriptor::TYPE_SINT32:
        case FieldDescriptor::TYPE_INT32:    _pref->SetInt32(&message, _pfdesc, TypeCast<int32_t>(_field)); return true;
        case FieldDescriptor::TYPE_FIXED64:
        case FieldDescriptor::TYPE_UINT64:   _pref->SetUInt64(&message, _pfdesc, TypeCast<uint64_t>(_field)); return true;
        case FieldDescriptor::TYPE_FIXED32:
        case FieldDescriptor::TYPE_UINT32:   _pref->SetUInt32(&message, _pfdesc, TypeCast<uint32_t>(_field)); return true;
        case FieldDescriptor::TYPE_BOOL:     _pref->SetBool(&message, _pfdesc, StringToBool(_field)); return true;
        case FieldDescriptor::TYPE_STRING:   _pref->SetString(&message, _pfdesc, _field); return true;
        case FieldDescriptor::TYPE_ENUM: {
            // get enum descriptor
            auto const* pedesc = _pfdesc->enum_type();
            // translate to en name
            if (!NameTableMap::Instance()->TranslateEnumName(pedesc->full_name(), _field)) return false;
            // find enum value descriptor
            auto const* pevdesc = pedesc->FindValueByName(_field);
            if (pevdesc == nullptr) {
                SetErrString("cant enum value: " + pedesc->full_name() + " " + _field);
                LSF_ERR << ErrString() << std::endl;
                return false;
            }
            // set enum
            _pref->SetEnum(&message, _pfdesc, pevdesc);
            return true;
        }
        case FieldDescriptor::TYPE_MESSAGE: {
            // get mutable message
            auto* p_sub_msg = _pref->MutableMessage(&message, _pfdesc);
            // recursive process
            return ProcessField(*p_sub_msg);
        }
        case FieldDescriptor::TYPE_BYTES:
        case FieldDescriptor::TYPE_GROUP:
        default: {
            ostringstream oss;
            oss << "unknown field type: " << _pfdesc->full_name() << " " << _pfdesc->type();
            SetErrString(oss.str());
            LSF_ERR << ErrString() << std::endl;
            return false;
        }
    }
}

bool CSVConfigProcessor::ProcessSetRepeatedField(Message& message) {
    // get size
    auto field_size = _pref->FieldSize(message, _pfdesc);
    switch (_pfdesc->type()) {
        case FieldDescriptor::TYPE_DOUBLE:   _pref->SetRepeatedDouble(&message, _pfdesc, field_size-1, TypeCast<double>(_field)); return true;
        case FieldDescriptor::TYPE_FLOAT:    _pref->SetRepeatedFloat(&message, _pfdesc, field_size-1, TypeCast<float>(_field)); return true;
        case FieldDescriptor::TYPE_SFIXED64:
        case FieldDescriptor::TYPE_SINT64:
        case FieldDescriptor::TYPE_INT64:    _pref->SetRepeatedInt64(&message, _pfdesc, field_size-1, TypeCast<int64_t>(_field)); return true;
        case FieldDescriptor::TYPE_SFIXED32:
        case FieldDescriptor::TYPE_SINT32:
        case FieldDescriptor::TYPE_INT32:    _pref->SetRepeatedInt32(&message, _pfdesc, field_size-1, TypeCast<int32_t>(_field)); return true;
        case FieldDescriptor::TYPE_FIXED64:
        case FieldDescriptor::TYPE_UINT64:   _pref->SetRepeatedUInt64(&message, _pfdesc, field_size-1, TypeCast<uint64_t>(_field)); return true;
        case FieldDescriptor::TYPE_FIXED32:
        case FieldDescriptor::TYPE_UINT32:   _pref->SetRepeatedUInt32(&message, _pfdesc, field_size-1, TypeCast<uint32_t>(_field)); return true;
        case FieldDescriptor::TYPE_BOOL:     _pref->SetRepeatedBool(&message, _pfdesc, field_size-1, StringToBool(_field)); return true;
        case FieldDescriptor::TYPE_STRING:   _pref->SetRepeatedString(&message, _pfdesc, field_size-1, _field); return true;
        case FieldDescriptor::TYPE_ENUM: {
            // get enum descriptor
            auto const* pedesc = _pfdesc->enum_type();
            // translate to en name
            if (!NameTableMap::Instance()->TranslateEnumName(pedesc->full_name(), _field)) return false;
            // find enum value descriptor
            auto const* pevdesc = pedesc->FindValueByName(_field);
            if (pevdesc == nullptr) {
                SetErrString("cant enum value: " + pedesc->full_name() + " " + _field);
                LSF_ERR << ErrString() << std::endl;
                return false;
            }
            // set enum
            _pref->SetRepeatedEnum(&message, _pfdesc, field_size-1, pevdesc);
            return true;
        }
        case FieldDescriptor::TYPE_MESSAGE: {
            // get mutable message
            auto* p_sub_msg = _pref->MutableRepeatedMessage(&message, _pfdesc, field_size-1);
            // recursive process
            return ProcessField(*p_sub_msg);
        }
        case FieldDescriptor::TYPE_BYTES:
        case FieldDescriptor::TYPE_GROUP:
        default: {
            ostringstream oss;
            oss << "unknown field type: " << _pfdesc->full_name() << " " << _pfdesc->type();
            SetErrString(oss.str());
            LSF_ERR << ErrString() << std::endl;
            return false;
        }
    }
}

bool CSVConfigProcessor::ProcessAddRepeatedField(Message& message) {
    switch (_pfdesc->type()) {
        case FieldDescriptor::TYPE_DOUBLE:   _pref->AddDouble(&message, _pfdesc, TypeCast<double>(_field)); return true;
        case FieldDescriptor::TYPE_FLOAT:    _pref->AddFloat(&message, _pfdesc, TypeCast<float>(_field)); return true;
        case FieldDescriptor::TYPE_SFIXED64:
        case FieldDescriptor::TYPE_SINT64:
        case FieldDescriptor::TYPE_INT64:    _pref->AddInt64(&message, _pfdesc, TypeCast<int64_t>(_field)); return true;
        case FieldDescriptor::TYPE_SFIXED32:
        case FieldDescriptor::TYPE_SINT32:
        case FieldDescriptor::TYPE_INT32:    _pref->AddInt32(&message, _pfdesc, TypeCast<int32_t>(_field)); return true;
        case FieldDescriptor::TYPE_FIXED64:
        case FieldDescriptor::TYPE_UINT64:   _pref->AddUInt64(&message, _pfdesc, TypeCast<uint64_t>(_field)); return true;
        case FieldDescriptor::TYPE_FIXED32:
        case FieldDescriptor::TYPE_UINT32:   _pref->AddUInt32(&message, _pfdesc, TypeCast<uint32_t>(_field)); return true;
        case FieldDescriptor::TYPE_BOOL:     _pref->AddBool(&message, _pfdesc, StringToBool(_field)); return true;
        case FieldDescriptor::TYPE_STRING:   _pref->AddString(&message, _pfdesc, _field); return true;
        case FieldDescriptor::TYPE_ENUM: {
            // get enum descriptor
            auto const* pedesc = _pfdesc->enum_type();
            // translate to en name
            if (!NameTableMap::Instance()->TranslateEnumName(pedesc->full_name(), _field)) return false;
            // find enum value descriptor
            auto const* pevdesc = pedesc->FindValueByName(_field);
            if (pevdesc == nullptr) {
                SetErrString("cant enum value: " + pedesc->full_name() + " " + _field);
                LSF_ERR << ErrString() << std::endl;
                return false;
            }
            // set enum
            _pref->AddEnum(&message, _pfdesc,  pevdesc);
            return true;
        }
        case FieldDescriptor::TYPE_MESSAGE: {
            // get mutable message
            auto* p_sub_msg = _pref->AddMessage(&message, _pfdesc);
            // recursive process
            return ProcessField(*p_sub_msg);
        }
        case FieldDescriptor::TYPE_BYTES:
        case FieldDescriptor::TYPE_GROUP:
        default: {
            ostringstream oss;
            oss << "unknown field type: " << _pfdesc->full_name() << " " << _pfdesc->type();
            SetErrString(oss.str());
            LSF_ERR << ErrString() << std::endl;
            return false;
        }
    }
}

std::string CSVConfigProcessor::GetFirstToken() {
    auto field_name = StringExt::SplitGet(_name, NAME_DELIMIT, 0);
    _name.erase(0, field_name.size());
    if (!_name.empty()) _name.erase(0, 1);
    return field_name;
}

bool CSVConfigProcessor::StringToBool(std::string const& field) {
    if (field == "是") return true;
    if (field == "否") return false;
    return TypeCast<int>(field) != 0;
}

// vim:ts=4:sw=4:et:ft=cpp:
