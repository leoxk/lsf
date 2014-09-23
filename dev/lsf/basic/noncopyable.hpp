// File:        noncopyable.hpp
// Description: ---
// Notes:       ---
// Author:      leoxiang <leoxiang@tencent.com>
// Revision:    2012-06-06 by leoxiang

#pragma once

namespace lsf {
namespace basic {

class NonCopyable
{
protected:
    NonCopyable()  { }
    ~NonCopyable() { }

private:
    NonCopyable(NonCopyable const &);                // copy constructor is hidden
    NonCopyable & operator=(NonCopyable const &);    // copy operator is hidden
};

} // end of namespace basic
} // end of namespace lsf

// vim:ts=4:sw=4:et:ft=cpp:
