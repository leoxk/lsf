// File:        type_cast.hpp
// Description: ---
// Notes:       ---
// Author:      leoxiang <leoxiang727@qq.com>
// Revision:    2012-01-31 by leoxiang

#pragma once

#include <string>
#include <sstream>
#include <cstdlib>

namespace lsf {
namespace basic {

////////////////////////////////////////////////////////////
// default use force type convert
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
_LSF_TYPE_CAST_FORCE_CONVERT(float)
_LSF_TYPE_CAST_FORCE_CONVERT(double)
_LSF_TYPE_CAST_FORCE_CONVERT(char)
_LSF_TYPE_CAST_FORCE_CONVERT(std::string const&)
_LSF_TYPE_CAST_FORCE_CONVERT(char const*)

#undef _LSF_TYPE_CAST_FORCE_CONVERT

////////////////////////////////////////////////////////////
// template specialization, string to number
////////////////////////////////////////////////////////////
#define _LSF_TYPE_CAST_STR_TO_ALL(type, func, args...)     \
    template <>                                            \
    inline type TypeCast<type>(std::string const& input) { \
        try { return (type)func(input, ##args); }          \
        catch (std::exception& e) { return type(); }       \
    }                                                      \
    template <>                                            \
    inline type TypeCast<type>(char const* input) {        \
        try { return (type)func(input, ##args); }          \
        catch (std::exception& e) { return type(); }       \
    }

_LSF_TYPE_CAST_STR_TO_ALL(int8_t,   std::stol, nullptr, 0);
_LSF_TYPE_CAST_STR_TO_ALL(int16_t,  std::stol, nullptr, 0);
_LSF_TYPE_CAST_STR_TO_ALL(int32_t,  std::stol, nullptr, 0);
_LSF_TYPE_CAST_STR_TO_ALL(int64_t,  std::stoll);
_LSF_TYPE_CAST_STR_TO_ALL(uint8_t,  std::stoul, nullptr, 0);
_LSF_TYPE_CAST_STR_TO_ALL(uint16_t, std::stoul, nullptr, 0);
_LSF_TYPE_CAST_STR_TO_ALL(uint32_t, std::stoul, nullptr, 0);
_LSF_TYPE_CAST_STR_TO_ALL(uint64_t, std::stoull, nullptr, 0);
_LSF_TYPE_CAST_STR_TO_ALL(float, std::stof);
_LSF_TYPE_CAST_STR_TO_ALL(double, std::stod);

#undef _LSF_TYPE_CAST_STR_TO_ALL

////////////////////////////////////////////////////////////
// template specialization, number to string
////////////////////////////////////////////////////////////
#define _LSF_TYPE_CAST_ALL_TO_STR(type)                    \
    template <>                                            \
    inline std::string TypeCast<std::string>(type input) { \
        return std::to_string(input);                      \
    }                                                      \

_LSF_TYPE_CAST_ALL_TO_STR(int8_t)
_LSF_TYPE_CAST_ALL_TO_STR(int16_t)
_LSF_TYPE_CAST_ALL_TO_STR(int32_t)
_LSF_TYPE_CAST_ALL_TO_STR(int64_t)
_LSF_TYPE_CAST_ALL_TO_STR(uint8_t)
_LSF_TYPE_CAST_ALL_TO_STR(uint16_t)
_LSF_TYPE_CAST_ALL_TO_STR(uint32_t)
_LSF_TYPE_CAST_ALL_TO_STR(uint64_t)
_LSF_TYPE_CAST_ALL_TO_STR(char)
_LSF_TYPE_CAST_ALL_TO_STR(float)
_LSF_TYPE_CAST_ALL_TO_STR(double)

#undef _LSF_TYPE_CAST_ALL_TO_STR

}  // end of namespace basic
}  // end of namespace lsf

// vim:ts=4:sw=4:et:ft=cpp:
