// File:        timer_manager.h
// Description: ---
// Notes:       ---
// Author:      leoxiang <leoxiang727@qq.com>
// Revision:    2015-06-29 by leoxiang

#pragma once

#include <queue>
#include <vector>
#include <algorithm>
#include "svr/common/basic_manager.h"
#include "svr/proto/data_mem.pb.h"

////////////////////////////////////////////////////////////
// PokerTimer
class Timer : public data::Timer
{
public:
    typedef data::Timer base_type;
    typedef decltype(base_type().timer_id()) key_type;
    typedef decltype(base_type().timer_milli_seconds()) time_type;
    typedef std::pair<time_type,key_type> pair_type;

public:
    bool Serialize(void * buf, size_t buflen, size_t & uselen);
    bool UnSerialize(void * buf, size_t buflen, size_t &uselen);
    size_t GetSize() const { return base_type::ByteSize(); }
    size_t GetKey() const { return base_type::timer_id(); }
    pair_type ToPair() const { return pair_type(base_type::timer_milli_seconds(), base_type::timer_id()); }
};

////////////////////////////////////////////////////////////
// TimerManager
class TimerManager : public BasicManager<Timer>, public lsf::basic::Singleton<TimerManager>
{
public:
    typedef BasicManager<Timer> base_type;
    typedef Timer::pair_type pair_type;
    typedef std::priority_queue<pair_type,std::vector<pair_type>,std::greater<pair_type>> heap_type;
    static const size_t MAX_TIMER_ID = 0xffffff;

public:
    bool Init(uint32_t shm_key, uint32_t max_size);
    Timer * CreateTimer(uint64_t milli_second, data::ENTimerType timer_type);
    Timer * GetTimer(uint32_t timer_id);
    void ReleaseTimer(uint32_t timer_id);
    void Tick();
    // void OnTimerTrigger(Timer * 

private:
	heap_type _heap;
	uint32_t _cur_max_id;
};



// vim:ts=4:sw=4:et:ft=cpp:
