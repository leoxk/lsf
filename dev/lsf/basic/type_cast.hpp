// File:        type_cast.hpp
// Description: ---
// Notes:       ---
// Author:      leoxiang <leoxiang727@qq.com>
// Revision:    2012-01-31 by leoxiang

#pragma once

#include <stdint.h>
#include <cstdlib>
#include <cstdio>
#include <string>
#include <sstream>

namespace lsf {
namespace basic {

////////////////////////////////////////////////////////////
// support all input type
// do not use 2 tempalte arg,
// because c++ standard do not allow function's partial-specialization
////////////////////////////////////////////////////////////
#define _LSF_TYPE_CAST_FORCE_CONVERT(type) \
    template <typename OutType>            \
    inline OutType TypeCast(type input) {  \
        return (OutType)input;             \
    }

_LSF_TYPE_CAST_FORCE_CONVERT(int8_t)
_LSF_TYPE_CAST_FORCE_CONVERT(int16_t)
_LSF_TYPE_CAST_FORCE_CONVERT(int32_t)
_LSF_TYPE_CAST_FORCE_CONVERT(int64_t)

_LSF_TYPE_CAST_FORCE_CONVERT(uint8_t)
_LSF_TYPE_CAST_FORCE_CONVERT(uint16_t)
_LSF_TYPE_CAST_FORCE_CONVERT(uint32_t)
_LSF_TYPE_CAST_FORCE_CONVERT(uint64_t)

_LSF_TYPE_CAST_FORCE_CONVERT(char)
_LSF_TYPE_CAST_FORCE_CONVERT(std::string const &)
_LSF_TYPE_CAST_FORCE_CONVERT(char const *)

#undef _LSF_TYPE_CAST_FORCE_CONVERT

////////////////////////////////////////////////////////////
// below is template specialization
////////////////////////////////////////////////////////////
// string and char * to number
#define _LSF_TYPE_CAST_STR_TO_ALL(type, func)              \
    template <>                                            \
    inline type TypeCast<type>(std::string const &input) { \
        return (type)std::func(input.c_str(), nullptr, 0);    \
    }                                                      \
    template <>                                            \
    inline type TypeCast<type>(char const *input) {        \
        return (type)std::func(input, nullptr, 0);            \
    }

_LSF_TYPE_CAST_STR_TO_ALL(int8_t, strtol);
_LSF_TYPE_CAST_STR_TO_ALL(int16_t, strtol);
_LSF_TYPE_CAST_STR_TO_ALL(int32_t, strtol);
_LSF_TYPE_CAST_STR_TO_ALL(int64_t, strtoll);

_LSF_TYPE_CAST_STR_TO_ALL(uint8_t, strtoul);
_LSF_TYPE_CAST_STR_TO_ALL(uint16_t, strtoul);
_LSF_TYPE_CAST_STR_TO_ALL(uint32_t, strtoul);
_LSF_TYPE_CAST_STR_TO_ALL(uint64_t, strtoull);

#undef _LSF_TYPE_CAST_STR_TO_ALL

////////////////////////////////////////////////////////////
// number to string and char *
////////////////////////////////////////////////////////////
#define _LSF_TYPE_CAST_ALL_TO_STR(type, flag)                     \
    template <>                                                   \
    inline std::string TypeCast<std::string>(type input) {        \
        char tmp[32];                                             \
        snprintf(tmp, sizeof(tmp), "%" #flag, input);             \
        return tmp;                                               \
    }                                                             \
    template <>                                                   \
    inline char const *TypeCast<char const *>(type input) {       \
        return TypeCast<std::string>(input).c_str();              \
    }                                                             \
    template <>                                                   \
    inline char *TypeCast<char *>(type input) {                   \
        return const_cast<char *>(TypeCast<char const *>(input)); \
    }

_LSF_TYPE_CAST_ALL_TO_STR(int8_t, d)
_LSF_TYPE_CAST_ALL_TO_STR(int16_t, d)
_LSF_TYPE_CAST_ALL_TO_STR(int32_t, d)
#if __WORDSIZE == 32
_LSF_TYPE_CAST_ALL_TO_STR(int64_t, lld)
#else
_LSF_TYPE_CAST_ALL_TO_STR(int64_t, ld)
#endif

_LSF_TYPE_CAST_ALL_TO_STR(uint8_t, u)
_LSF_TYPE_CAST_ALL_TO_STR(uint16_t, u)
_LSF_TYPE_CAST_ALL_TO_STR(uint32_t, u)
#if __WORDSIZE == 32
_LSF_TYPE_CAST_ALL_TO_STR(uint64_t, llu)
#else
_LSF_TYPE_CAST_ALL_TO_STR(uint64_t, lu)
#endif

_LSF_TYPE_CAST_ALL_TO_STR(char, c)

template <>
inline char *TypeCast<char *>(std::string const &input) {
    return const_cast<char *>(input.c_str());
}

template <>
inline char const *TypeCast<char const *>(std::string const &input) {
    return input.c_str();
}

template <>
inline std::string TypeCast<std::string>(char const *input) {
    return std::string(input);
}

#undef _LSF_TYPE_CAST_ALL_TO_STR

}  // end of namespace basic
}  // end of namespace lsf

// vim:ts=4:sw=4:et:ft=cpp:
