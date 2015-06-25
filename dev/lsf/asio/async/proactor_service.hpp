// File:        proactor_service.hpp
// Description: ---
// Notes:       ---
// Author:      leoxiang <leoxiang727@qq.com>
// Revision:    2012-06-12 by leoxiang

#pragma once

#include <sys/timerfd.h>
#include <sys/time.h>
#include <string>
#include <utility>
#include "lsf/basic/noncopyable.hpp"
#include "lsf/basic/error.hpp"
#include "lsf/basic/macro.hpp"
#include "lsf/asio/detail/basic_socket.hpp"
#include "lsf/asio/detail/basic_listen_socket.hpp"
#include "lsf/asio/async/completion_queue.hpp"
#include "lsf/asio/async/epoll_event_driver.hpp"
#include "lsf/asio/async/poll_event_driver.hpp"

namespace lsf {
namespace asio {
namespace async {

////////////////////////////////////////////////////////////
// AsyncInfo
////////////////////////////////////////////////////////////
class AsyncInfo {
public:
    detail::BasicSocket<> socket;
    detail::BasicListenSocket<> listen_socket;
    std::string buffer;
    int timer_fd = -1;
};

////////////////////////////////////////////////////////////
// ProactorSerivce
////////////////////////////////////////////////////////////
class ProactorSerivce : public basic::NonCopyable, public basic::Error {
public:
    typedef std::function<void()> tick_func_type;
    typedef std::function<void()> exit_func_type;

    static const size_t MAX_WAIT_MILLI_SECONDS = 10;
    static const size_t MAX_BUFFER_LEN = 128 * 1024;

public:
    ProactorSerivce(BasicEventDriver* pdriver) : _pdriver(pdriver) {}

    ////////////////////////////////////////////////////////////
    // Async Accept
    template <typename SocketType, typename HandlerType>
    bool AsyncAccept(SocketType& socket, HandlerType const& handler) {
        // register event
        if (!_pdriver->RegisterEvent(socket.GetSockFd(), BasicEventDriver::FLAG_READ)) {
            ErrString() = LSF_DEBUG_INFO + _pdriver->ErrString();
            return false;
        }

        // add completion task
        if (!_queue.AddCompletionTask(socket.GetSockFd(), CompletionFunc::ACTION_ACCEPT, handler)) {
            _pdriver->CancelEvent(socket.GetSockFd());
            ErrString() = LSF_DEBUG_INFO + _queue.ErrString();
            return false;
        }

        return true;
    }

    ////////////////////////////////////////////////////////////
    // Async Connect
    template <typename SocketType, typename SockAddrType, typename HandlerType>
    bool AsyncConnect(SocketType& socket, SockAddrType const& sockaddr, HandlerType const& handler) {
        // try connect
        int ret = ::connect(socket.GetSockFd(), sockaddr.data(), sockaddr.size());
        if (ret != 0 && errno != EINPROGRESS) {
            ErrString() = LSF_DEBUG_INFO + socket.ErrString();
            return false;
        }

        // register event
        if (!_pdriver->RegisterEvent(socket.GetSockFd(), BasicEventDriver::FLAG_WRITE)) {
            ErrString() = LSF_DEBUG_INFO + _pdriver->ErrString();
            return false;
        }

        // add completion task
        if (!_queue.AddCompletionTask(socket.GetSockFd(), CompletionFunc::ACTION_CONNECT, handler)) {
            _pdriver->CancelEvent(socket.GetSockFd());
            ErrString() = LSF_DEBUG_INFO + _queue.ErrString();
            return false;
        }

        return true;
    }

    ////////////////////////////////////////////////////////////
    // Async  Recv
    template <typename SocketType, typename HandlerType1, typename HandlerType2>
    bool AsyncRecv(SocketType& socket, HandlerType1 const& handler, HandlerType2 const& peer_close_handler) {
        return AsyncRead(socket, handler, peer_close_handler);
    }

    template <typename SocketType, typename HandlerType1, typename HandlerType2>
    bool AsyncRead(SocketType& socket, HandlerType1 const& handler, HandlerType2 const& peer_close_handler) {
        // call same routine
        if (!AsyncRead(socket, handler)) return false;

        // add peer close handler
        if (!_queue.AddCompletionTask(socket.GetSockFd(), CompletionFunc::ACTION_PEER_CLOSE, peer_close_handler)) {
            _pdriver->CancelEvent(socket.GetSockFd());
            ErrString() = LSF_DEBUG_INFO + _queue.ErrString();
            return false;
        }

        return true;
    }

    template <typename SocketType, typename HandlerType>
    bool AsyncRead(SocketType& socket, HandlerType const& handler) {
        // register event
        if (!_pdriver->RegisterEvent(socket.GetSockFd(), BasicEventDriver::FLAG_READ)) {
            ErrString() = LSF_DEBUG_INFO + _pdriver->ErrString();
            return false;
        }

        // add completion task
        if (!_queue.AddCompletionTask(socket.GetSockFd(), CompletionFunc::ACTION_READ, handler)) {
            _pdriver->CancelEvent(socket.GetSockFd());
            ErrString() = LSF_DEBUG_INFO + _queue.ErrString();
            return false;
        }

        return true;
    }

    ////////////////////////////////////////////////////////////
    // Async Send
    template <typename SocketType, typename HandlerType>
    bool AsyncSend(SocketType& socket, std::string const& buffer, HandlerType const& handler) {
        return AsyncWrite(socket, buffer.c_str(), buffer.length(), handler);
    }

    template <typename SocketType, typename HandlerType>
    bool AsyncSend(SocketType& socket, void const* buffer, size_t buflen, HandlerType const& handler) {
        return AsyncWrite(socket, buffer, buflen, handler);
    }

    template <typename SocketType, typename HandlerType>
    bool AsyncWrite(SocketType& socket, std::string const& buffer, HandlerType const& handler) {
        return AsyncWrite(socket, buffer.c_str(), buffer.length(), handler);
    }

    template <typename SocketType, typename HandlerType>
    bool AsyncWrite(SocketType& socket, void const* buffer, size_t buflen, HandlerType const& handler) {
        // register event
        if (!_pdriver->RegisterEvent(socket.GetSockFd(), BasicEventDriver::FLAG_WRITE)) {
            ErrString() = LSF_DEBUG_INFO + _pdriver->ErrString();
            return false;
        }

        // add completion task
        if (!_queue.AddCompletionTask(socket.GetSockFd(), CompletionFunc::ACTION_WRITE, handler, buffer, buflen)) {
            _pdriver->CancelEvent(socket.GetSockFd());
            ErrString() = LSF_DEBUG_INFO + _queue.ErrString();
            return false;
        }

        return true;
    }

    ////////////////////////////////////////////////////////////
    // Async Timer
    template <typename HandlerType>
    bool AsyncAddTimerSingle(uint64_t milli_expire, HandlerType const& handler) {
        return AsyncAddTimer(milli_expire, 0, 1, handler);
    }

    template <typename HandlerType>
    bool AsyncAddTimerForever(uint64_t milli_expire, HandlerType const& handler) {
        return AsyncAddTimer(milli_expire, milli_expire, 0, handler);
    }

    template <typename HandlerType>
    bool AsyncAddTimerMulti(uint64_t milli_expire, size_t trigger_count, HandlerType const& handler) {
        return AsyncAddTimer(milli_expire, milli_expire, trigger_count, handler);
    }

    template <typename HandlerType>
    bool AsyncAddTimer(uint64_t milli_expire, uint64_t milli_interval, size_t trigger_count, HandlerType const& handler) {
        // check input
        if (milli_expire == 0) milli_expire = 1;

        // create timer
        int fd = ErrWrap(::timerfd_create(CLOCK_REALTIME, TFD_NONBLOCK));
        if (fd < 0) return false;

        // set timeout
        itimerspec timer_info;
        timer_info.it_value.tv_sec = milli_expire / 1000;
        timer_info.it_value.tv_nsec = (milli_expire % 1000) * 1000000;
        timer_info.it_interval.tv_sec = milli_interval / 1000;
        timer_info.it_interval.tv_nsec = (milli_interval % 1000) * 1000000;
        if (ErrWrap(::timerfd_settime(fd, 0, &timer_info, nullptr)) < 0) {
            ::close(fd);
            return false;
        }

        // register event
        if (!_pdriver->RegisterEvent(fd, BasicEventDriver::FLAG_READ)) {
            ::close(fd);
            ErrString() = LSF_DEBUG_INFO + _pdriver->ErrString();
            return false;
        }

        // add completion task
        if (!_queue.AddCompletionTask(fd, CompletionFunc::ACTION_TIMER, handler, nullptr, 0, trigger_count)) {
            _pdriver->CancelEvent(fd);
            ::close(fd);
            ErrString() = LSF_DEBUG_INFO + _queue.ErrString();
            return false;
        }

        return true;
    }

    void AsyncDelTimer(int fd) {
        // clear timer
        itimerspec timer_info;
        memset(&timer_info, 0, sizeof(timer_info));
        ::timerfd_settime(fd, 0, &timer_info, nullptr);

        // clear async
        CloseAsync(fd);
        ::close(fd);
    }

    bool AsyncGetTimer(int fd, uint64_t* p_expire, uint64_t* p_interval = nullptr) {
        // get timer
        itimerspec timer_info;
        if (ErrWrap(::timerfd_gettime(fd, &timer_info)) < 0) return false;

        if (p_expire) *p_expire = timer_info.it_value.tv_sec * 1000 + timer_info.it_value.tv_nsec / 1000;
        if (p_interval) *p_interval = timer_info.it_interval.tv_sec * 1000 + timer_info.it_interval.tv_nsec / 1000;
        return true;
    }

    ////////////////////////////////////////////////////////////
    // Close Async
    void CloseAsync(int fd) {
        // check input
        if (fd < 0) return;

        // cancel registration
        _pdriver->CancelEvent(fd);

        // cancel completion task
        _queue.CancelCompletionTask(fd);
    }

    ////////////////////////////////////////////////////////////
    // Main Routine
    void Run() {
        while (true) {
            int fd, flag;

            // update clock
            _UpdateClock();

            // wait events
            _pdriver->WaitEvent(MAX_WAIT_MILLI_SECONDS);

            // traverse ready events
            while (_pdriver->GetReadyEvent(&fd, &flag)) {
                // read action
                if (flag & BasicEventDriver::FLAG_READ) {
                    CompletionFunc* pfunc = nullptr;
                    if (!_queue.GetReadCompletionTask(fd, &pfunc)) continue;

                    switch (pfunc->action) {
                        case CompletionFunc::ACTION_ACCEPT:
                            OnAcceptEvent(fd);
                            break;
                        case CompletionFunc::ACTION_READ:
                            OnReadEvent(fd);
                            break;
                        case CompletionFunc::ACTION_TIMER:
                            OnTimerEvent(fd);
                            break;
                        default:
                            continue;
                    }
                }

                // write action
                if (flag & BasicEventDriver::FLAG_WRITE) {
                    CompletionFunc* pfunc = nullptr;
                    if (!_queue.GetWriteCompletionTask(fd, &pfunc)) continue;

                    switch (pfunc->action) {
                        case CompletionFunc::ACTION_WRITE:
                            OnWriteEvent(fd);
                            break;
                        case CompletionFunc::ACTION_CONNECT:
                            OnConnectEvent(fd);
                            break;
                        default:
                            continue;
                    }
                }

                // error handle
                if (flag & BasicEventDriver::FLAG_ERR) {
                    CloseAsync(fd);
                    ::close(fd);
                }
            }

            // call routine
            if (_tick_func) _tick_func();

            // check exit
            if (_is_exit) break;
        }

        // call exit func
        if (_exit_func) _exit_func();
    }

    ////////////////////////////////////////////////////////////
    // Accept Event
    void OnAcceptEvent(int fd) {
        detail::BasicListenSocket<> listen_socket(fd);
        AsyncInfo info;
        info.listen_socket = listen_socket;

        // accept
        detail::BasicSocket<> accept_socket(-1);
        if (!listen_socket.Accept(accept_socket)) {
            ErrString() = LSF_DEBUG_INFO + listen_socket.ErrString();
            return;
        }
        info.socket = accept_socket;

        // call accept handler
        CompletionFunc* pfunc = nullptr;
        if (!_queue.GetReadCompletionTask(fd, &pfunc)) {
            accept_socket.Close();
            return;
        }
        if (!pfunc->func(info)) {
            accept_socket.Close();
        }
    }

    ////////////////////////////////////////////////////////////
    // Read Event
    void OnReadEvent(int fd) {
        detail::BasicSocket<> socket(fd);
        AsyncInfo info;
        info.socket = socket;
        info.buffer.reserve(MAX_BUFFER_LEN);  // avoid copy

        // read loop
        bool close_connection = false;
        bool peer_close = false;
        while (true) {
            static char buffer[MAX_BUFFER_LEN];
            ssize_t ret = ::recv(socket.GetSockFd(), buffer, sizeof(buffer), 0);

            // handle error
            if (ret < 0) {
                if (errno == EINTR)  // signal interrupt
                {
                    continue;
                } else if (errno == EAGAIN || errno == EWOULDBLOCK)  // no data
                {
                    break;
                } else  // error
                {
                    close_connection = true;
                    break;
                }
            }

            // handle socket close
            if (ret == 0) {
                peer_close = true;
                close_connection = true;
                break;
            }

            // handle read data
            info.buffer.append(buffer, ret);
        }

        // call read handler first
        if (!info.buffer.empty()) {
            CompletionFunc* pfunc = nullptr;
            if (_queue.GetReadCompletionTask(fd, &pfunc)) {
                // set close if necessary
                if (!pfunc->func(info)) {
                    close_connection = true;
                }
            }
        }

        // call peer close handler
        if (peer_close) {
            CompletionFunc* pfunc = nullptr;
            if (_queue.GetPeerCloseCompletionTask(fd, &pfunc)) {
                pfunc->func(info);
            }
            CloseAsync(fd);
            ::close(fd);
            return;
        }

        // check close connection
        if (close_connection) {
            CloseAsync(fd);
            ::close(fd);
            return;
        }
    }

    ////////////////////////////////////////////////////////////
    // Write Event
    void OnWriteEvent(int fd) {
        detail::BasicSocket<> socket(fd);
        AsyncInfo info;
        info.socket = socket;

        // get completion func
        CompletionFunc* pfunc = nullptr;
        if (!_queue.GetWriteCompletionTask(fd, &pfunc)) {
            CloseAsync(fd);
            ::close(fd);
            return;
        }

        // write data
        size_t total = 0;
        bool close_connection = false;
        while (total < pfunc->buffer.length()) {
            int ret = ::send(socket.GetSockFd(), pfunc->buffer.data() + total, pfunc->buffer.length() - total, 0);

            // handle error
            if (ret < 0) {
                if (errno == EINTR || errno == EAGAIN || errno == EWOULDBLOCK)  // signal interrupt or not ready
                {
                    continue;
                } else  // error
                {
                    close_connection = true;
                    break;
                }
            }

            // handle send data
            total += ret;
        }

        // move buffer
        info.buffer = std::move(pfunc->buffer);

        // if send wrong
        if (close_connection) {
            CloseAsync(fd);
            ::close(fd);
        }
        // call write handler
        else if (!pfunc->func(info)) {
            CloseAsync(fd);
            ::close(fd);
        } else {
            // always cancel write registration after event
            CloseAsync(fd);
        }
    }

    ////////////////////////////////////////////////////////////
    // Connect Event
    void OnConnectEvent(int fd) {
        detail::BasicSocket<> socket(fd);
        AsyncInfo info;
        info.socket = socket;

        // get completion func
        CompletionFunc* pfunc = nullptr;
        if (!_queue.GetWriteCompletionTask(fd, &pfunc)) {
            CloseAsync(fd);
            ::close(fd);
            return;
        }

        // call write handler
        if (!pfunc->func(info)) {
            CloseAsync(fd);
            ::close(fd);
        } else {
            // always cancel write registration after event
            CloseAsync(fd);
        }
    }

    ////////////////////////////////////////////////////////////
    // Timer Event
    void OnTimerEvent(int fd) {
        AsyncInfo info;
        info.timer_fd = fd;

        // read data
        uint64_t count;
        int ret = ::read(fd, &count, sizeof(count));
        if (ret != sizeof(count)) {
            if (ret != EAGAIN) AsyncDelTimer(fd);
            return;
        }

        // get completion func
        CompletionFunc* pfunc = nullptr;
        if (!_queue.GetReadCompletionTask(fd, &pfunc)) {
            AsyncDelTimer(fd);
            return;
        }

        // call write handler as many as it triggers
        bool should_delete = false;
        for (uint64_t i = 0; i < count; ++i) {
            // callback
            if (!pfunc->func(info)) {
                AsyncDelTimer(fd);
                return;
            }
            // check remain count
            if (pfunc->timer_count == 1) {
                should_delete = true;
                break;
            }
            else if (pfunc->timer_count != 0) {
                pfunc->timer_count--;
            }
        }

        // check if need delete
        if (should_delete) {
            AsyncDelTimer(fd);
            return;
        }
        uint64_t expire;
        if (!AsyncGetTimer(fd, &expire)) {
            AsyncDelTimer(fd);
            return;
        }
        if (expire == 0) {
            AsyncDelTimer(fd);
        }
    }

    ////////////////////////////////////////////////////////////
    // Other Func
    template <typename HandlerType>
    void SetTickFunc(HandlerType const& handler) {
        _tick_func = tick_func_type(handler);
    }

    template <typename HandlerType>
    void SetExitFunc(HandlerType const& handler) {
        _exit_func = exit_func_type(handler);
    }

    void SetExit() { _is_exit = true; }

    uint64_t GetClockTime() const { return _ts.tv_sec + _ts.tv_nsec / 1000000000; }
    uint64_t GetClockTimeMilli() const { return _ts.tv_sec * 1000 + _ts.tv_nsec / 1000000; }
    uint64_t GetClockTimeMicro() const { return _ts.tv_sec * 1000000 + _ts.tv_nsec / 1000; }
    uint64_t GetClockTimeNano() const { return _ts.tv_sec * 1000000000 + _ts.tv_nsec; }

private:
    void _UpdateClock() { ::clock_gettime(CLOCK_REALTIME, &_ts); }

private:
    BasicEventDriver* _pdriver = nullptr;
    bool _is_exit = false;
    CompletionQueue _queue;
    tick_func_type _tick_func;
    exit_func_type _exit_func;
    timespec _ts = { 0, 0 };
};

}  // end of namespace async
}  // end of namespace asio
}  // end of namespace lsf

// vim:ts=4:sw=4:et:ft=cpp:
