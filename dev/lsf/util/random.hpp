// File:        random.hpp
// Description: ---
// Notes:       ---
// Author:      leoxiang <leoxiang727@qq.com>
// Revision:    2012-08-09 by leoxiang

#pragma once

#include <cstdlib>
#include <sys/time.h>
#include "lsf/basic/error.hpp"
#include "lsf/basic/singleton.hpp"

namespace lsf {
namespace util {

class Random : public lsf::basic::Error {
public:
    static const size_t MAX_RANDOM_COUNT = 10000;

public:
    Random(size_t max_cnt = MAX_RANDOM_COUNT) : _max_cnt(max_cnt) { InitSeed(); }

    void InitSeed() {
        timeval tv;
        ::gettimeofday(&tv, nullptr);
        ::srand(tv.tv_usec);
        _use_cnt = 0;
    }

    // generate random between [0, end]
    uint32_t GetRand(uint32_t end) { return GetRand(0, end); }

    // generate random between [start, end]
    uint32_t GetRand(uint32_t start, uint32_t end) {
        if (end <= start) return start;

        uint32_t off = end - start + 1;
        uint64_t random;

        if (_use_cnt > _max_cnt) InitSeed();

        if (off < 0xfff) {
            random = ::rand();
            _use_cnt++;
        } else if (off < 0xfffff) {
            random = (::rand() & 0xfff) | ((::rand() & 0xfff) << 24);
            _use_cnt++;
        } else {
            random = (::rand() & 0xfff) | ((::rand() & 0xfff) << 24) | ((uint64_t)(::rand() & 0xfff) << 48);
            _use_cnt++;
        }

        return start + random % off;
    }

    // accessor
    void SetMaxCount(size_t max_cnt) { _max_cnt = max_cnt; }

    size_t GetUseCount() const { return _use_cnt; }

private:
    size_t _max_cnt = 0;
    size_t _use_cnt = 0;
};

////////////////////////////////////////////////////////////
// Singleton Random
class SingleRandom : public Random, public lsf::basic::Singleton<Random> {
public:
    static uint32_t GetRand(uint32_t end) { return lsf::basic::Singleton<Random>::Reference().GetRand(end); }

    static uint32_t GetRand(uint32_t start, uint32_t end) { return lsf::basic::Singleton<Random>::Reference().GetRand(start, end); }
};

}  // end of namespace util
}  // end of namespace lsf

// vim:ts=4:sw=4:et:ft=cpp:
