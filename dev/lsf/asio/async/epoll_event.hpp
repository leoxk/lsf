// File:        epoll_event.hpp
// Description: ---
// Notes:       ---
// Author:      leoxiang <leoxiang727@qq.com>
// Revision:    2012-06-12 by leoxiang

#pragma once

#include <sys/epoll.h>
#include "lsf/basic/noncopyable.hpp"
#include "lsf/basic/error.hpp"

namespace lsf {
namespace asio {
namespace async {
    
class EpollEvent : 
    public basic::NonCopyable, 
    public basic::Error
{
public:
    static const int FLAG_READ  = 0x1;
    static const int FLAG_WRITE = 0x2;
    static const int FLAG_ERR   = 0x4;
    static const int FLAG_PRI   = 0x8;
    static const int FLAG_RDHUP = 0x10;

    static const size_t MAX_EV_NUM = 128;

public:
    EpollEvent() : _max_evs(0), _cur_pos(0) {
        _epfd = ErrWrap(::epoll_create(128000));
    }

    bool RegisterEvent(int fd, int flag) {
        if (!(flag & FLAG_READ) && !(flag & FLAG_WRITE)) return false;

        epoll_event ev = { 0, { 0 } };
        // here we use EPOLLRDHUP to make epoll aware of peer close connection 
        // or shutdown write-half of the connection, see epoll_ctl for more
        if (flag & FLAG_READ)  ev.events |= EPOLLIN;
        if (flag & FLAG_WRITE) ev.events |= EPOLLOUT;
        if (flag & FLAG_ERR)   ev.events |= EPOLLHUP | EPOLLERR;;
        if (flag & FLAG_PRI)   ev.events |= EPOLLPRI;
        if (flag & FLAG_RDHUP) ev.events |= EPOLLRDHUP;
        ev.data.fd = fd;

        int res = ErrWrap(::epoll_ctl(_epfd, EPOLL_CTL_MOD, fd, &ev));
        if (res == 0) return true;
        if (errno == ENOENT) return ErrWrap(::epoll_ctl(_epfd, EPOLL_CTL_ADD, fd, &ev)) == 0;
        return false;
    }

    bool CancelEvent(int fd) {
        epoll_event ev = { 0, { 0 } };
        return ErrWrap(::epoll_ctl(_epfd, EPOLL_CTL_DEL, fd, &ev));
    }

    bool WaitEvent(int timeout) {
        _cur_pos = 0;
        int ret = ErrWrap(::epoll_wait(_epfd, _evs, MAX_EV_NUM, timeout));
        _max_evs = std::max(0, ret);
        return ret >= 0;
    }

    bool GetReadyEvent(int * pfd, int * pflag) {
        if (_cur_pos >= _max_evs) return false;

        *pflag = 0;
        if (_evs[_cur_pos].events & EPOLLIN)    *pflag |= FLAG_READ;
        if (_evs[_cur_pos].events & EPOLLOUT)   *pflag |= FLAG_WRITE;
        if (_evs[_cur_pos].events & EPOLLERR)   *pflag |= FLAG_ERR;
        if (_evs[_cur_pos].events & EPOLLHUP)   *pflag |= FLAG_ERR;
        if (_evs[_cur_pos].events & EPOLLPRI)   *pflag |= FLAG_PRI;
        if (_evs[_cur_pos].events & EPOLLRDHUP) *pflag |= FLAG_RDHUP;

        *pfd = _evs[_cur_pos].data.fd;

        _cur_pos++;
        return true;
    }

    int GetEpollFd() const { return _epfd; }

private:
    int         _epfd;
    epoll_event _evs[MAX_EV_NUM];
    size_t      _max_evs;
    size_t      _cur_pos;
};

} // end of namespace async
} // end of namespace asio
} // end of namespace lsf

// vim:ts=4:sw=4:et:ft=cpp:
