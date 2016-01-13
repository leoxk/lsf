// File:        timer_manager.cpp
// Description: ---
// Notes:       ---
// Author:      leoxiang <leoxiang727@qq.com>
// Revision:    2015-06-29 by leoxiang

#include "svr/common/timer_manager.h"
#include "svr/common/common_header.h"

using namespace lsf::asio;
using namespace lsf::basic;
using namespace lsf::util;

////////////////////////////////////////////////////////////
// Timer
bool Timer::Serialize(void* buf, size_t buflen, size_t& uselen) const {
    if (!lsf::util::SerializeProtobuf(buf, buflen, uselen, *this)) return false;
    return true;
}

bool Timer::UnSerialize(void* buf, size_t buflen, size_t& uselen) {
    if (!lsf::util::UnSerializeProtobuf(buf, buflen, uselen, *this)) return false;
    return true;
}

std::string Timer::ToString() const {
    static std::string tmp;
    tmp = "timer_id=" + TypeCast<std::string>(base_type::timer_id()) +
          ", timer_type=" + ProtoEnumToString(base_type::timer_type());
    return tmp;
}

////////////////////////////////////////////////////////////
// TimerManager
bool TimerManager::Init(uint32_t shm_key, uint32_t max_size) {
    // call base init
    if (!base_type::Init(shm_key, max_size)) return false;

    // recover max id
    _cur_max_id = 0;
    for (auto& pair : *this) {
        _cur_max_id = std::max(_cur_max_id, pair.second.timer_id());
        _timer_heap.push(pair.second.ToPair());
    }

    // register tick callback
    IOService::Instance()->AsyncAddTimerForever(DEF_TIMER_TICK_INTERVAL,
            std::bind(&TimerManager::OnTick, this));

    return true;
}

Timer* TimerManager::CreateTimer(uint64_t milli_second, data::ENTimerType timer_type) {
    // check full
    if (base_type::full()) {
        LSF_LOG_ERR("timer is full, size=%d, max_size=%d", base_type::size(), base_type::max_size());
        return nullptr;
    }

    // find a valid id
    uint32_t timer_id;
    do { timer_id = (++_cur_max_id) & MAX_TIMER_ID; } while (base_type::exist(timer_id));

    // create timer
    Timer& timer = base_type::operator[](timer_id);

    // init
    timer.set_timer_id(timer_id);
    timer.set_timer_type(timer_type);
    timer.set_timer_milli_seconds(IOService::Instance()->ClockTimeMilli() + milli_second);

    // put into heap
    _timer_heap.push(timer.ToPair());

    return &timer;
}

void TimerManager::ReleaseTimer(uint32_t timer_id) {
    // set delete flag
    Timer* ptimer = GetTimer(timer_id);
    if (ptimer) ptimer->set_is_delete(true);
}

Timer* TimerManager::GetTimer(uint32_t timer_id) {
    auto iter = base_type::find(timer_id);
    return iter == base_type::end() ? nullptr : &iter->second;
}

void TimerManager::OnTick() {
    // get top until found not expire timer
    std::vector<Timer> timer_trigger_list;
    while (!_timer_heap.empty()) {
        // check time
        pair_type const& pair = _timer_heap.top();
        if (pair.first > IOService::Instance()->ClockTimeMilli()) break;

        // scope exit logic
        LSF_SCOPE_EXIT() {
            base_type::erase(pair.second);
            _timer_heap.pop();
        };

        // get timer
        Timer* ptimer = GetTimer(pair.second);
        if (ptimer == nullptr) {
            LSF_LOG_FAT("timer not exist but heap say yes, timer_id=%u", pair.second);
            continue;
        }

        // if delete just pop
        if (ptimer->is_delete()) continue;

        // find handler
        if (!data::ENTimerType_IsValid(ptimer->timer_type()) || !_func_arr[ptimer->timer_type()]) {
            LSF_LOG_ERR("timer type invalid or not register, %s", LSF_ETS(ptimer->timer_type()));
            continue;
        }

        // add to vec
        timer_trigger_list.push_back(*ptimer);
    }

    // call timer handle
    for (auto& timer : timer_trigger_list) {
        _func_arr[timer.timer_type()](timer);
    }
}

// vim:ts=4:sw=4:et:ft=cpp:
