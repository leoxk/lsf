// File:        noncopyable.hpp
// Description: ---
// Notes:       ---
// Author:      leoxiang <leoxiang727@qq.com>
// Revision:    2012-06-06 by leoxiang

#pragma once

namespace lsf {
namespace basic {

class NonCopyable
{
public:
    NonCopyable() = default;
    ~NonCopyable() = default;

private:
    NonCopyable(NonCopyable const &) = delete;                  // copy constructor is hidden
    NonCopyable & operator=(NonCopyable const &) = delete;      // copy operator is hidden
};

} // end of namespace basic
} // end of namespace lsf

// vim:ts=4:sw=4:et:ft=cpp:
