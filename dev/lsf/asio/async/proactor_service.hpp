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
#include "lsf/util/log.hpp"
#include "lsf/asio/detail/basic_socket.hpp"
#include "lsf/asio/detail/basic_listen_socket.hpp"
#include "lsf/asio/async/completion_queue.hpp"
#include "lsf/asio/async/epoll_event_driver.hpp"
#include "lsf/asio/async/poll_event_driver.hpp"

namespace lsf {
namespace asio {
namespace async {

////////////////////////////////////////////////////////////
// ProactorSerivce
////////////////////////////////////////////////////////////
class ProactorSerivce : public lsf::basic::NonCopyable, public lsf::basic::Error {
public:
    using tick_func_type = std::function<void()>;
    using exit_func_type = std::function<void()>;

    static const size_t MAX_WAIT_MILLI_SECONDS = 10;
    static const size_t MAX_BUFFER_LEN = 128 * 1024;

public:
    ProactorSerivce(BasicEventDriver* pdriver) : _pdriver(pdriver) {}

    ////////////////////////////////////////////////////////////
    // Async Accept
    template <typename SocketType, typename HandlerType>
    bool AsyncAccept(SocketType& socket, HandlerType&& handler) {
        // register event
        if (!_pdriver->RegisterEvent(socket.GetSockFd(), BasicEventDriver::FLAG_READ)) {
            ErrString() = LSF_DEBUG_INFO + _pdriver->ErrString();
            return false;
        }

        // add completion task
        ReadCompletionFunc& read_func = _queue.GetAndCreateReadCompletionFunc(socket.GetSockFd());
        read_func.action = ReadCompletionFunc::ACTION_ACCEPT;
        read_func.accept_func = std::forward<HandlerType>(handler);

        return true;
    }

    ////////////////////////////////////////////////////////////
    // Async Connect
    template <typename SocketType, typename SockAddrType, typename HandlerType1, typename HandlerType2 = std::nullptr_t>
    bool AsyncConnect(SocketType& socket, SockAddrType&& sockaddr, HandlerType1&& handler, HandlerType2&& fail_handler = nullptr) {
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
        WriteCompletionFunc& write_func = _queue.GetAndCreateWriteCompletionFunc(socket.GetSockFd());
        write_func.action = WriteCompletionFunc::ACTION_CONNECT;
        write_func.connect_func = std::forward<HandlerType1>(handler);
        write_func.connect_fail_func = std::forward<HandlerType2>(fail_handler);
        write_func.sockaddr = std::forward<SockAddrType>(sockaddr);

        return true;
    }

    ////////////////////////////////////////////////////////////
    // Async Send
    LSF_FUNC_ALIAS(AsyncWrite, AsyncSend)

    template <typename SocketType, typename StringType, typename HandlerType>
    bool AsyncWrite(SocketType& socket, StringType&& buffer, HandlerType&& handler) {
        // register event
        if (!_pdriver->RegisterEvent(socket.GetSockFd(), BasicEventDriver::FLAG_WRITE)) {
            ErrString() = LSF_DEBUG_INFO + _pdriver->ErrString();
            return false;
        }

        // add completion task
        WriteCompletionFunc& write_func = _queue.GetAndCreateWriteCompletionFunc(socket.GetSockFd());
        write_func.action = WriteCompletionFunc::ACTION_WRITE;
        write_func.write_func = std::forward<HandlerType>(handler);
        write_func.buffer = std::forward<StringType>(buffer);

        return true;
    }

    ////////////////////////////////////////////////////////////
    // Async  Recv
    LSF_FUNC_ALIAS(AsyncRead, AsycRecv)

    template <typename SocketType, typename HandlerType1, typename HandlerType2 = std::nullptr_t>
    bool AsyncRead(SocketType& socket, HandlerType1&& handler, HandlerType2&& peer_close_handler = nullptr) {
        // register event
        if (!_pdriver->RegisterEvent(socket.GetSockFd(), BasicEventDriver::FLAG_READ)) {
            ErrString() = LSF_DEBUG_INFO + _pdriver->ErrString();
            return false;
        }

        // add completion task
        ReadCompletionFunc& read_func = _queue.GetAndCreateReadCompletionFunc(socket.GetSockFd());
        read_func.action = ReadCompletionFunc::ACTION_READ;
        read_func.read_func = std::forward<HandlerType1>(handler);
        read_func.read_peer_close_func = std::forward<HandlerType2>(peer_close_handler);

        return true;
    }

    ////////////////////////////////////////////////////////////
    // Async Timer
    template <typename HandlerType>
    bool AsyncAddTimerSingle(uint64_t milli_expire, HandlerType&& handler) {
        return AsyncAddTimer(milli_expire, 0, 1, std::forward<HandlerType>(handler));
    }

    template <typename HandlerType>
    bool AsyncAddTimerForever(uint64_t milli_expire, HandlerType&& handler) {
        return AsyncAddTimer(milli_expire, milli_expire, 0, std::forward<HandlerType>(handler));
    }

    template <typename HandlerType>
    bool AsyncAddTimerMulti(uint64_t milli_expire, size_t trigger_count, HandlerType&& handler) {
        return AsyncAddTimer(milli_expire, milli_expire, trigger_count, std::forward<HandlerType>(handler));
    }

    template <typename HandlerType>
    bool AsyncAddTimer(uint64_t milli_expire, uint64_t milli_interval, size_t trigger_count, HandlerType&& handler) {
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
        ReadCompletionFunc& read_func = _queue.GetAndCreateReadCompletionFunc(fd);
        read_func.action = WriteCompletionFunc::ACTION_TIMER;
        read_func.timer_func = std::forward<HandlerType>(handler);
        read_func.timer_count = trigger_count;

        return true;
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
    template<typename SocketType>
    void AsyncClose(SocketType&& socket) { AsyncClose(socket.GetSockFd()); }

    void AsyncClose(int fd) {
        // cancel async
        AsyncCancel(fd);

        // close fd
        ::close(fd);
    }

    void AsyncCancel(int fd) {
        // check input
        if (fd < 0) return;

        // cancel registration
        _pdriver->CancelEvent(fd);

        // cancel completion task
        _queue.CancelCompletionFunc(fd);
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
                    ReadCompletionFunc* pfunc = _queue.GetReadCompletionFunc(fd);
                    if (pfunc) {
                        switch (pfunc->action) {
                            case ReadCompletionFunc::ACTION_ACCEPT: OnAcceptEvent(fd, pfunc); break;
                            case ReadCompletionFunc::ACTION_READ: OnReadEvent(fd, pfunc); break;
                            case ReadCompletionFunc::ACTION_TIMER: OnTimerEvent(fd, pfunc); break;
                            default: break;
                        }
                    }
                    else {
                        LSF_LOG_FATAL("cant get read handler but event triggers, fd=%u", fd);
                        AsyncClose(fd);
                    }
                }

                // write action
                if (flag & BasicEventDriver::FLAG_WRITE) {
                    WriteCompletionFunc* pfunc = _queue.GetWriteCompletionFunc(fd);
                    if (pfunc) {
                        switch (pfunc->action) {
                            case WriteCompletionFunc::ACTION_WRITE: OnWriteEvent(fd, pfunc); break;
                            case WriteCompletionFunc::ACTION_CONNECT: OnConnectEvent(fd, pfunc); break;
                            default: break;
                        }
                    }
                    else {
                        LSF_LOG_FATAL("cant get write handler buf event triggers, fd=%u", fd);
                        AsyncClose(fd);
                    }
                }

                // error handle
                if (flag & BasicEventDriver::FLAG_ERR) {
                    OnErrorEvent(fd);
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
    void OnAcceptEvent(int fd, ReadCompletionFunc* pfunc) {
        detail::BasicListenSocket<> listen_socket(fd);

        // accept
        detail::BasicSocket<> accept_socket(-1);
        if (!listen_socket.Accept(accept_socket)) {
            ErrString() = LSF_DEBUG_INFO + listen_socket.ErrString();
            return;
        }

        // call accept handler
        if (pfunc->accept_func && !pfunc->accept_func(accept_socket, listen_socket)) {
            AsyncClose(accept_socket.GetSockFd());
        }
    }

    ////////////////////////////////////////////////////////////
    // Connect Event
    void OnConnectEvent(int fd, WriteCompletionFunc* pfunc) {
        detail::BasicSocket<> socket(fd);

        // check ret
        int ret = socket.GetSockError();
        if (ret != 0) {
            SetErrorNo(ret);
            if (pfunc->connect_fail_func) pfunc->connect_fail_func(socket, pfunc->sockaddr);
            AsyncClose(fd);
            return;
        }

        // call write handler
        if (pfunc->connect_func && !pfunc->connect_func(socket, pfunc->sockaddr)) {
            AsyncClose(fd);
            return;
        }

        // always cancel write registration after event
        AsyncCancel(fd);
    }

    ////////////////////////////////////////////////////////////
    // Write Event
    void OnWriteEvent(int fd, WriteCompletionFunc* pfunc) {
        detail::BasicSocket<> socket(fd);

        // write data
        size_t total = 0;
        bool close_connection = false;
        while (total < pfunc->buffer.length()) {
            int ret = ::send(socket.GetSockFd(), pfunc->buffer.data() + total, pfunc->buffer.length() - total, 0);

            // handle error
            if (ret < 0) {
                if (errno == EINTR || errno == EAGAIN || errno == EWOULDBLOCK) { // signal interrupt or not ready
                    continue;
                }
                else { // error
                    close_connection = true;
                    break;
                }
            }

            // handle send data
            total += ret;
        }

        // if send wrong
        if (close_connection) {
            AsyncClose(fd);
            return;
        }

        // call write handler
        if (pfunc->write_func && !pfunc->write_func(socket, pfunc->buffer)) {
            AsyncClose(fd);
            return;
        }

        // always cancel write registration after event
        AsyncCancel(fd);
    }

    ////////////////////////////////////////////////////////////
    // Read Event
    void OnReadEvent(int fd, ReadCompletionFunc* pfunc) {
        detail::BasicSocket<> socket(fd);
        std::string buffer;

        // read loop
        bool close_connection = false;
        bool peer_close = false;
        while (true) {
            char tmp[MAX_BUFFER_LEN];
            ssize_t ret = ::recv(socket.GetSockFd(), tmp, sizeof(tmp), 0);

            // handle error
            if (ret < 0) {
                if (errno == EINTR) { // signal interrupt
                    continue;
                } else if (errno == EAGAIN || errno == EWOULDBLOCK) { // no data
                    break;
                } else { // error
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
            buffer.append(tmp, ret);
        }

        // handle read first
        if (!buffer.empty() && pfunc->read_func && !pfunc->read_func(socket, buffer)) {
            close_connection = true;
        }

        // handle peer close
        if (peer_close) {
            if (pfunc->read_peer_close_func) pfunc->read_peer_close_func(socket);
            AsyncClose(fd);
            return;
        }

        // handle close connection
        if (close_connection) {
            AsyncClose(fd);
            return;
        }
    }

    ////////////////////////////////////////////////////////////
    // Timer Event
    void OnTimerEvent(int fd, ReadCompletionFunc* pfunc) {
        // read data
        uint64_t count;
        int ret = ::read(fd, &count, sizeof(count));
        if (ret != sizeof(count)) { // return error
            // not block reason then close
            if (ret != EAGAIN) AsyncClose(fd);
            return;
        }

        // call write handler as many as it triggers
        bool should_delete = false;
        for (uint64_t i = 0; i < count; ++i) {
            // callback
            if (pfunc->timer_func && !pfunc->timer_func(fd)) {
                AsyncClose(fd);
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
            AsyncClose(fd);
            return;
        }
        uint64_t expire;
        if (!AsyncGetTimer(fd, &expire)) {
            AsyncClose(fd);
            return;
        }
        if (expire == 0) AsyncClose(fd);
    }

    ////////////////////////////////////////////////////////////
    // Error Event
    void OnErrorEvent(int fd) {
        // check connect fail action
        WriteCompletionFunc* pfunc = _queue.GetWriteCompletionFunc(fd);
        if (pfunc != nullptr && pfunc->action == WriteCompletionFunc::ACTION_CONNECT) {
            detail::BasicSocket<> socket(fd);
            int ret = socket.GetSockError();
            SetErrorNo(ret);
            if (pfunc->connect_fail_func) pfunc->connect_fail_func(socket, pfunc->sockaddr);
        }

        // close
        AsyncClose(fd);
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
    bool              _is_exit = false;
    timespec          _ts      = {0,0};
    CompletionQueue   _queue;
    tick_func_type    _tick_func;
    exit_func_type    _exit_func;
};

}  // end of namespace async
}  // end of namespace asio
}  // end of namespace lsf

// vim:ts=4:sw=4:et:ft=cpp:
