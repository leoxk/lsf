// File:        epoll_event_driver.hpp
// Description: ---
// Notes:       ---
// Author:      leoxiang <leoxiang727@qq.com>
// Revision:    2012-06-12 by leoxiang

#pragma once

#include <sys/epoll.h>
#include "lsf/asio/async/basic_event_driver.hpp"

namespace lsf {
namespace asio {
namespace async {

////////////////////////////////////////////////////////////
// EpollEvent
////////////////////////////////////////////////////////////
class EpollEventDriver : public BasicEventDriver {
public:
    static const size_t MAX_EV_NUM = 128;

public:
    EpollEventDriver() { _epfd = ErrWrap(::epoll_create(0)); } // this size is not used nowadays, see man 

    bool RegisterEvent(int fd, int flag) {
        if (!(flag & FLAG_READ) && !(flag & FLAG_WRITE)) return false;

        epoll_event ev = {0, {0}};
        ev.data.fd = fd;
        ev.events |= (EPOLLERR | EPOLLHUP);
        // here we use EPOLLRDHUP to make epoll aware of peer close connection
        // or shutdown write-half of the connection, see epoll_ctl for more
        if (flag & FLAG_READ) ev.events |= EPOLLIN;
        if (flag & FLAG_WRITE) ev.events |= EPOLLOUT;

        int res = ErrWrap(::epoll_ctl(_epfd, EPOLL_CTL_MOD, fd, &ev));
        if (res == 0) return true;
        if (errno == ENOENT) return ErrWrap(::epoll_ctl(_epfd, EPOLL_CTL_ADD, fd, &ev)) == 0;
        return false;
    }

    void CancelEvent(int fd) { ::epoll_ctl(_epfd, EPOLL_CTL_DEL, fd, NULL); }

    bool WaitEvent(int timeout) {
        _cur_pos = 0;
        int ret = ErrWrap(::epoll_wait(_epfd, _evs, MAX_EV_NUM, timeout));
        _max_evs = std::max(0, ret);
        return ret >= 0;
    }

    bool GetReadyEvent(int* pfd, int* pflag) {
        if (_cur_pos >= _max_evs) return false;

        *pflag = 0;
        if (_evs[_cur_pos].events & EPOLLIN) *pflag |= FLAG_READ;
        if (_evs[_cur_pos].events & EPOLLOUT) *pflag |= FLAG_WRITE;
        if (_evs[_cur_pos].events & EPOLLERR) *pflag |= FLAG_ERR;
        if (_evs[_cur_pos].events & EPOLLHUP) *pflag |= FLAG_ERR;

        *pfd = _evs[_cur_pos].data.fd;

        _cur_pos++;
        return true;
    }

private:
    int _epfd = 0;
    size_t _max_evs = 0;
    size_t _cur_pos = 0;
    epoll_event _evs[MAX_EV_NUM];
};

}  // end of namespace async
}  // end of namespace asio
}  // end of namespace lsf

// vim:ts=4:sw=4:et:ft=cpp:
