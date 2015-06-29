// File:        timer_manager.cpp
// Description: ---
// Notes:       ---
// Author:      leoxiang <leoxiang727@qq.com>
// Revision:    2015-06-29 by leoxiang

#include "svr/common/timer_manager.h"
#include "svr/common/common_header.h"

using namespace lsf::asio;

////////////////////////////////////////////////////////////
// Timer
bool Timer::Serialize(void * buf, size_t buflen, size_t & uselen) {
    if (!lsf::util::SerializeProtobuf(buf, buflen, uselen, *this)) return false;
    return true;
}

bool Timer::UnSerialize(void * buf, size_t buflen, size_t &uselen) {
    if (!lsf::util::UnSerializeProtobuf(buf, buflen, uselen, *this)) return false;
    return true;
}

////////////////////////////////////////////////////////////
// TimerManager
bool TimerManager::Init(uint32_t shm_key, uint32_t max_size) {
    // call base init
    if (!base_type::Init(shm_key, max_size)) return false;

    // recover max id
    _cur_max_id = 0;
    for (auto const & pair : *this) {
        _cur_max_id = std::max(_cur_max_id, pair.second.timer_id());
        _heap.push(pair.second.ToPair());
    }

    return true;
}

Timer * TimerManager::CreateTimer(uint64_t milli_second, data::ENTimerType timer_type) {
    // check full
    if (base_type::full()) {
        LSF_LOG_ERR("timer is full, size=%d, max_size=%d", base_type::size(), base_type::max_size());
        return nullptr;
    }

    // find a valid id
    uint32_t timer_id;
    do { timer_id = ++_cur_max_id & MAX_TIMER_ID; } while (base_type::exist(timer_id));

    // create timer
    Timer & timer = base_type::operator[](timer_id);

    // init
    timer.set_timer_id(timer_id);
    timer.set_timer_type(timer_type);
    timer.set_timer_milli_seconds(IOService::Instance()->GetClockTimeMilli() + milli_second);

    // put into heap
    _heap.push(timer.ToPair());

    return &timer;
}

void TimerManager::ReleaseTimer(uint32_t timer_id) {
    // set delete flag
    Timer * ptimer = GetTimer(timer_id);
    if (ptimer) ptimer->set_is_delete(true);
}

Timer * TimerManager::GetTimer(uint32_t timer_id) {
    auto iter = base_type::find(timer_id);
    return iter == base_type::end() ? nullptr : &iter->second;
}

void TimerManager::Tick() {
    // get top until found not expire timer
    while (!_heap.empty()) {
        // check time
        pair_type const & pair = _heap.top();
        if (pair.first > IOService::Instance()->GetClockTimeMilli()) break;

        // get timer
        Timer * ptimer = GetTimer(pair.second);
        if (ptimer == nullptr) {
            LSF_LOG_FATAL("timer not exist but heap say yes, timer_id=%u", pair.second);
            _heap.pop();
            continue;
        }

        // if not delete then trigger TODO
        if (!ptimer->is_delete()) {

        }

        // pop
        _heap.pop();
    }
}


// vim:ts=4:sw=4:et:ft=cpp:
