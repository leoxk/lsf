// File:        test_file_lock.cpp
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

#include "lsf/basic/unit_test.hpp"
#include "lsf/util/file_lock.hpp"
#include "lsf/util/system.hpp"

using namespace std;
using namespace lsf::util;

#define FILE_PATH "./test.lock"
LSF_TEST_CASE(test_file_lock)
{
    // lock file
    FileLock locker;
    LSF_ASSERT(locker.Lock(FILE_PATH));
    LSF_ASSERT(locker.IsLocked());

    // another locker try to lock. should failed
    FileLock locker2;
    LSF_ASSERT(!locker2.Lock(FILE_PATH));
    LSF_ASSERT(!locker2.IsLocked());

    // after locker1 release lock, locker2 can lock
    LSF_ASSERT(locker.UnLock());
    LSF_ASSERT(locker2.Lock(FILE_PATH));
    LSF_ASSERT(locker2.IsLocked());

    // delete file
    LSF_ASSERT(System::Rm(FILE_PATH));
}

int main(int argc, char **argv)
{
	LSF_TEST_ALL();
}

// vim:ts=4:sw=4:et:ft=cpp:
