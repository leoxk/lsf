// File:        file_lock.hpp
// Description: ---
// Notes:       ---
// Author:      leoxiang <xiangkun@ximigame.com>
// Revision:    2014-09-24 by leoxiang
//
// Copyright (C) 1998-2014 XiMi. All Rights Reserved.
// This is unpublished proprietary source code of XiMi Ltd. The copyright
// notice above does not evidence any actual or intended publication of such
// source code. UNAUTHORIZED DISTRIBUTION, ADAPTATION OR USE MAY BE SUBJECT
// TO CIVIL AND CRIMINAL PENALTIES.

#pragma once

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <fcntl.h>
#include "lsf/basic/error.hpp"

namespace lsf {
namespace util {

class FileLock : public basic::Error
{
public:
    FileLock() : _lockfd(-1) { }
        
    ~FileLock() { UnLock(); }
    
    bool Lock(char const * lockfile)
    {
        if (IsLocked()) return false;

        if ((_lockfd = ErrWrap(::open(lockfile, O_RDWR | O_CREAT, 0644))) < 0) return false;

        if (ErrWrap(::flock(_lockfd, LOCK_EX | LOCK_NB)) < 0) 
        {
            ::close(_lockfd); 
            _lockfd = -1;
            return false;
        }
        
        return true;
    }

    bool UnLock()
    {
        if (!IsLocked()) return false;
        ::close(_lockfd); 
        _lockfd = -1;
        return true;
    }

    bool IsLocked() { return _lockfd != -1; }

private:
    int _lockfd;
};

} // end of namespace util
} // end of namespace lsf

// vim:ts=4:sw=4:et:ft=cpp:
