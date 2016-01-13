// File:        macro.hpp
// Description: ---
// Notes:       ---
// Author:      leoxiang <leoxiang727@qq.com>
// Revision:    2012-12-10 by leoxiang

#pragma once
#include "lsf/basic/error.hpp"
#include "lsf/basic/type_cast.hpp"

////////////////////////////////////////////////////////////
// token to string
#define LSF_TOKEN_CAT(x, y) __LSF_TOKEN_CAT1(x, y)
#define LSF_TOKEN_TO_STRING(x) __LSF_TOKEN_TO_STRING1(x)
#define __LSF_TOKEN_CAT1(x, y) x##y
#define __LSF_TOKEN_TO_STRING1(x) #x

////////////////////////////////////////////////////////////
// debug
#define LSF_DEBUG_INFO "[" __FILE__ "|" LSF_TOKEN_TO_STRING(__LINE__) "] "

////////////////////////////////////////////////////////////
// alias func
#define LSF_FUNC_ALIAS(name, aname) template<typename... Args> \
auto name(Args&&... args) -> decltype(f(std::forward<Args>(args)...)) { return aname(std::forward<Args>(args)...); }

////////////////////////////////////////////////////////////
// enum to char string
#define LSF_ENUM_TO_STRING_PROTO(EnumType)                  \
namespace lsf {                                             \
namespace util {                                            \
inline std::string const& ProtoEnumToString(EnumType e) {   \
    return LSF_TOKEN_CAT(EnumType,_Name)(e);                \
}                                                           \
}                                                           \
}                                                           \

////////////////////////////////////////////////////////////
#define LSF_ETS(e)  lsf::util::ProtoEnumToString(e).c_str()

////////////////////////////////////////////////////////////
// class to char string
#define LSF_TS(obj) (obj).ToString().c_str()

////////////////////////////////////////////////////////////
// error string
#define LSF_ES(obj) (obj).ErrString().c_str()

////////////////////////////////////////////////////////////
// system error string
#define LSF_SES() lsf::basic::Error::SysErrString().c_str()

////////////////////////////////////////////////////////////
// protobuf to char string
#define LSF_PTS(msg) lsf::util::Protobuf::MsgToTextFormatString(msg).c_str()

// vim:ts=4:sw=4:et:ft=cpp:
