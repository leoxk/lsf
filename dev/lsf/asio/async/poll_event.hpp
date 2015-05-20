// File:        poll_event.hpp
// Description: ---
// Notes:       ---
// Author:      leoxiang <leoxiang727@qq.com>
// Revision:    2012-06-13 by leoxiang

#pragma once

#include <poll.h>
#include "lsf/basic/noncopyable.hpp"
#include "lsf/basic/error.hpp"

namespace lsf {
namespace asio {
namespace async {
    
class PollEvent : 
    public basic::NonCopyable, 
    public basic::Error
{
public:
    static const int FLAG_READ  = 0x1;
    static const int FLAG_WRITE = 0x2;
    static const int FLAG_ERR   = 0x4;
    static const int FLAG_PRI   = 0x8;
    static const int FLAG_RDHUP = 0x10;

    static const size_t MAX_FD_NUM = 1024;

public:
    PollEvent() : _fds_size(0) {
        memset(_fds, 0, sizeof(_fds));
    }

    bool RegisterEvent(int fd, int flag) {
        if (!(flag & FLAG_READ) && !(flag & FLAG_WRITE)) return false;

        if (_fds_size >= MAX_FD_NUM) return false;

        // check existance
        size_t pos;
        for (pos = 0; pos < _fds_size; ++pos)
        {
            if (_fds[pos].fd == fd) break;
        }

        _fds[pos].fd = fd;
        // here we use EPOLLRDHUP to make epoll aware of peer close connection 
        // or shutdown write-half of the connection, see epoll_ctl for more
        if (flag & FLAG_READ)  _fds[pos].events |= POLLIN;
        if (flag & FLAG_WRITE) _fds[pos].events |= POLLOUT;
        if (flag & FLAG_ERR)   _fds[pos].events |= POLLHUP | POLLERR | POLLNVAL;
        if (flag & FLAG_PRI)   _fds[pos].events |= POLLPRI;
        if (flag & FLAG_RDHUP) _fds[pos].events |= POLLRDHUP;

        if (pos == _fds_size) _fds_size++;
        return true;
    }

    bool CancelEvent(int fd) {
        size_t pos;
        for (pos = 0; pos < MAX_FD_NUM; pos++) 
            if (_fds[pos].fd == fd) break;

        if (pos == MAX_FD_NUM) return false;
        // use memmove instead of memcpy, for overlap reason
        ::memmove(_fds + pos, _fds + pos + 1, (_fds_size - 1 - pos) * sizeof(pollfd));

        _fds_size--;
        return true;
    }

    bool WaitEvent(int timeout) {
        _cur_pos = 0;
        if (ErrWrap(::poll(_fds, _fds_size, timeout) < 0)) 
            return false;
        else
            return true;
    }

    bool GetReadyEvent(int * pfd, int * pflag) {
        if (_cur_pos >= _fds_size) return false;

        for ( ; _cur_pos < _fds_size; _cur_pos++)
            if (_fds[_cur_pos].revents != 0) break;

        if (_cur_pos >= _fds_size) return false;

        *pflag = 0;
        if (_fds[_cur_pos].revents & POLLIN)    *pflag |= FLAG_READ;
        if (_fds[_cur_pos].revents & POLLOUT)   *pflag |= FLAG_WRITE;
        if (_fds[_cur_pos].revents & POLLERR)   *pflag |= FLAG_ERR;
        if (_fds[_cur_pos].revents & POLLHUP)   *pflag |= FLAG_ERR;
        if (_fds[_cur_pos].revents & POLLNVAL)  *pflag |= FLAG_ERR;
        if (_fds[_cur_pos].revents & POLLPRI)   *pflag |= FLAG_PRI;
        if (_fds[_cur_pos].revents & POLLRDHUP) *pflag |= FLAG_RDHUP;

        *pfd = _fds[_cur_pos].fd;

        _cur_pos++;
        return true;
    }

private:
    pollfd        _fds[MAX_FD_NUM];
    size_t        _fds_size;
    size_t        _cur_pos;
};

} // end of namespace async
} // end of namespace asio
} // end of namespace lsf

// vim:ts=4:sw=4:et:ft=cpp:
