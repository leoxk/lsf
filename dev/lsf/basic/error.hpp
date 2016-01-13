// File:        error.hpp
// Description: ---
// Notes:       ---
// Author:      leoxiang <leoxiang727@qq.com>
// Revision:    2012-06-06 by leoxiang

#pragma once

#include <cerrno>
#include <cstring>
#include <string>

namespace lsf {
namespace basic {

////////////////////////////////////////////////////////////
// Error
////////////////////////////////////////////////////////////
class Error {
public:
    inline static std::string SysErrString()  { return std::strerror(errno); }

public:
protected:
    template <typename AnyType>
    inline AnyType ErrWrap(AnyType expr) const {
        if ((int)expr < 0) _err_string = SysErrString();
        return expr;
    }

    template <typename AnyType>
    inline AnyType ErrWrapPointer(AnyType expr) const {
        if (!_err_string.empty()) _err_string.clear();
        return expr;
    }

    inline void SetErrString(char const* char_str)   const { _err_string = char_str; }
    inline void SetErrString(std::string const& str) const { _err_string = str; }

public:
    inline std::string const& ErrString() const { return _err_string; }

protected:
    mutable std::string _err_string;
};

}  // end of namespace basic
}  // end of namespace lsf

// G_FATAL
// vim:ts=4:sw=4:et:ft=cpp:
