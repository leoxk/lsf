// File:        file_lock.hpp
// Description: ---
// Notes:       ---
// Author:      leoxiang <leoxiang727@qq.com>
// Revision:    2014-09-24 by leoxiang

#pragma once

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <fcntl.h>
#include <unistd.h>
#include "lsf/basic/error.hpp"

namespace lsf {
namespace util {

class FileLock : public basic::Error {
public:
    ~FileLock() { UnLock(); }

    bool Lock(char const* lockfile) {
        if (IsLocked()) return false;

        if ((_lockfd = ErrWrap(::open(lockfile, O_RDWR | O_CREAT, 0644))) < 0) return false;

        if (ErrWrap(::flock(_lockfd, LOCK_EX | LOCK_NB)) < 0) {
            ::close(_lockfd);
            _lockfd = -1;
            return false;
        }

        return true;
    }

    bool UnLock() {
        if (!IsLocked()) return false;
        ::close(_lockfd);
        _lockfd = -1;
        return true;
    }

    bool IsLocked() { return _lockfd != -1; }

private:
    int _lockfd = -1;
};

}  // end of namespace util
}  // end of namespace lsf

// vim:ts=4:sw=4:et:ft=cpp:
