// File:        time.hpp
// Description: ---
// Notes:       ---
// Author:      leoxiang <leoxiang727@qq.com>
// Revision:    2015-06-22 by leoxiang

#pragma once

#include <sys/time.h>
#include <cstdint>

namespace lsf {
namespace basic {

class Time {
public:
    static uint64_t TimeValToMilli(struct timeval const & tv) {
        return tv.tv_sec * 1000 + tv.tv_usec / 1000;
    }

    static int64_t TimeValDiff(struct timeval const & tv1, struct timeval const & tv2) {
        return TimeValToMilli(tv1) - TimeValToMilli(tv2);
    }

    static uint64_t Now() {
        struct timeval tv;
        ::gettimeofday(&tv, nullptr);
        return TimeValToMilli(tv);
    }
};

}  // end of namespace basic
} // end of namespace lsf

// vim:ts=4:sw=4:et:ft=cpp:
