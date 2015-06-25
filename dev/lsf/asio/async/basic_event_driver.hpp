// File:        basic_event_driver.hpp
// Description: ---
// Notes:       ---
// Author:      leoxiang <leoxiang727@qq.com>
// Revision:    2015-06-11 by leoxiang

#pragma once

#include "lsf/basic/noncopyable.hpp"
#include "lsf/basic/error.hpp"

namespace lsf {
namespace asio {
namespace async {

class BasicEventDriver : public lsf::basic::NonCopyable, public lsf::basic::Error {
public:
    static const int FLAG_READ = 0x1;
    static const int FLAG_WRITE = 0x2;
    static const int FLAG_ERR = 0x4;

public:
    virtual bool RegisterEvent(int fd, int flag) = 0;
    virtual void CancelEvent(int fd) = 0;
    virtual bool WaitEvent(int timeout) = 0;
    virtual bool GetReadyEvent(int* pfd, int* pflag) = 0;
};

}  // end of namespace async
}  // end of namespace asio
}  // end of namespace lsf

// vim:ts=4:sw=4:et:ft=cpp:
