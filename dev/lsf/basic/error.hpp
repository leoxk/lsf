// File:        error.hpp
// Description: ---
// Notes:       ---
// Author:      leoxiang <leoxiang@tencent.com>
// Revision:    2012-06-06 by leoxiang

#pragma once

#include <cerrno>
#include <cstring>
#include <string>

namespace lsf {
namespace basic {

////////////////////////////////////////////////////////////
// ErrorStatic
////////////////////////////////////////////////////////////
namespace {

static std::string err_string;

} // end of anonymous namespace

class ErrorStatic
{
public:
    template<typename AnyType>
    static AnyType ErrWrap(AnyType expr) {
        if ((int)expr < 0) err_string = std::strerror(errno);
        return expr;
    }

    template<typename AnyType>
    static AnyType ErrWrapPointer(AnyType expr) {
        if (expr == NULL) err_string = std::strerror(errno);
        return expr;
    }

    static std::string & ErrString() { return err_string; }
};

////////////////////////////////////////////////////////////
// Error
////////////////////////////////////////////////////////////
class Error
{
public:
    static std::string SysErrString() { 
        return std::strerror(errno); 
    }

    template<typename AnyType> 
    AnyType ErrWrap(AnyType expr) const {
        if (!_err_string.empty()) _err_string.clear();
        if ((int)expr < 0) _err_string = SysErrString();
        return expr;
    }

    template<typename AnyType>
    AnyType ErrWrapPointer(AnyType expr) const {
        if (!_err_string.empty()) _err_string.clear();
        if (expr == NULL) err_string = SysErrString();
        return expr;
    }

    std::string const & ErrString() const { return _err_string; }
    std::string & ErrString() { return _err_string; }

protected:
    mutable std::string _err_string;
};

} // end of namespace basic
} // end of namespace lsf

// vim:ts=4:sw=4:et:ft=cpp:
