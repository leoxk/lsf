// File:        timer_manager.h
// Description: ---
// Notes:       ---
// Author:      leoxiang <leoxiang727@qq.com>
// Revision:    2015-06-29 by leoxiang

#pragma once
#include <queue>
#include <vector>
#include <map>
#include <functional>
#include <algorithm>
#include "svr/common/basic_manager.h"
#include "svr/proto/data_mem.pb.h"

////////////////////////////////////////////////////////////
// PokerTimer
class Timer : public data::Timer
{
public:
    using base_type = data::Timer;
    using key_type = decltype(base_type().timer_id());
    using time_type = decltype(base_type().timer_milli_seconds());
    using pair_type = std::pair<time_type,key_type>;

public:
    bool Serialize(void * buf, size_t buflen, size_t & uselen) const;
    bool UnSerialize(void * buf, size_t buflen, size_t &uselen);
    size_t GetSize() const { return base_type::ByteSize(); }
    size_t GetKey() const { return base_type::timer_id(); }
    pair_type ToPair() const { return pair_type(base_type::timer_milli_seconds(), base_type::timer_id()); }
    std::string ToString() const;
};

////////////////////////////////////////////////////////////
// TimerManager
class TimerManager : public BasicManager<Timer>, public lsf::basic::Singleton<TimerManager>
{
public:
    using base_type = BasicManager<Timer>;
    using pair_type = Timer::pair_type;
    using heap_type = std::priority_queue<pair_type,std::vector<pair_type>,std::greater<pair_type>>;

    using func_type = std::function<void(Timer const &)>;
    using func_map_type = std::map<data::ENTimerType,func_type>;
    static const size_t MAX_TIMER_ID = 0xffffff;

public:
    bool Init(uint32_t shm_key, uint32_t max_size);
    Timer * CreateTimer(uint64_t milli_second, data::ENTimerType timer_type);
    Timer * GetTimer(uint32_t timer_id);
    void ReleaseTimer(uint32_t timer_id);
    void Tick();

public:
    template<typename HandlerType>
    void AddTimerHandle(data::ENTimerType timer_type, HandlerType&& handler) {
        _func_map[timer_type] = std::forward<HandlerType>(handler);
    }

private:
	heap_type _timer_heap;
	uint32_t _cur_max_id;
    func_map_type _func_map;
};



// vim:ts=4:sw=4:et:ft=cpp:
