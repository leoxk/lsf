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
#include "lsf/asio/socket.hpp"
#include "lsf/asio/async/completion_queue.hpp"
#include "lsf/asio/async/epoll_event_driver.hpp"
#include "lsf/asio/async/poll_event_driver.hpp"
#include "lsf/asio/async/read_data_buffer.hpp"

namespace lsf {
namespace asio {
namespace async {

////////////////////////////////////////////////////////////
// ProactorSerivce
////////////////////////////////////////////////////////////
class ProactorSerivce : public lsf::basic::NonCopyable, public lsf::basic::Error {
public:
    static const size_t MAX_WAIT_MILLI_SECONDS = 10;
    static const size_t MAX_BUFFER_LEN = 128 * 1024;

public:
    ProactorSerivce(BasicEventDriver* pdriver) : _pdriver(pdriver) {}

    ////////////////////////////////////////////////////////////
    // Async Accept
    template <typename HandlerType1, typename HandlerType2 = std::nullptr_t>
    bool AsyncAccept(SharedSocket shared_socket, HandlerType1&& handler, HandlerType2&& fail_handler = nullptr) {
        // register event
        if (!_pdriver->RegisterEvent(shared_socket->SockFd(), BasicEventDriver::FLAG_READ)) {
            SetErrString(LSF_DEBUG_INFO + _pdriver->ErrString());
            return false;
        }

        // add completion task
        auto shared_completion = _queue.GetAndCreateReadCompletionFunc(shared_socket->SockFd());
        shared_completion->action = CompletionQueue::ACTION_ACCEPT;
        shared_completion->accept_func = std::forward<HandlerType1>(handler);
        shared_completion->accept_fail_func = std::forward<HandlerType2>(fail_handler);
        shared_completion->shared_socket = shared_socket;
        return true;
    }

    ////////////////////////////////////////////////////////////
    // Async Connect
    template <typename HandlerType1, typename HandlerType2 = std::nullptr_t>
    inline bool AsyncConnect(SharedSocket shared_socket, Address const& address, uint16_t port, HandlerType1&& handler, HandlerType2&& fail_handler = nullptr) {
        return AsyncConnect(shared_socket, SockAddr(address, port), handler, fail_handler);
    }

    template <typename HandlerType1, typename HandlerType2 = std::nullptr_t>
    bool AsyncConnect(SharedSocket shared_socket, SockAddr const& sockaddr, HandlerType1&& handler, HandlerType2&& fail_handler = nullptr) {
        // try connect
        int ret = ::connect(shared_socket->SockFd(), sockaddr.data(), sockaddr.size());
        if (ret != 0 && errno != EINPROGRESS) {
            SetErrString(LSF_DEBUG_INFO + SysErrString());
            return false;
        }

        // register event
        if (!_pdriver->RegisterEvent(shared_socket->SockFd(), BasicEventDriver::FLAG_WRITE)) {
            SetErrString(LSF_DEBUG_INFO + _pdriver->ErrString());
            return false;
        }

        // add completion task
        auto shared_completion = _queue.GetAndCreateWriteCompletionFunc(shared_socket->SockFd());
        shared_completion->action = CompletionQueue::ACTION_CONNECT;
        shared_completion->connect_func = std::forward<HandlerType1>(handler);
        shared_completion->connect_fail_func = std::forward<HandlerType2>(fail_handler);
        shared_completion->sockaddr = sockaddr;
        shared_completion->shared_socket = shared_socket;
        return true;
    }

    ////////////////////////////////////////////////////////////
    // Async Send
    LSF_FUNC_ALIAS(AsyncWrite, AsyncSend)

    template <typename StringType, typename HandlerType>
    bool AsyncWrite(SharedSocket shared_socket, StringType&& buffer, HandlerType&& handler) {
        // register event
        if (!_pdriver->RegisterEvent(shared_socket->SockFd(), BasicEventDriver::FLAG_WRITE)) {
            SetErrString(LSF_DEBUG_INFO + _pdriver->ErrString());
            return false;
        }

        // add completion task
        auto shared_completion = _queue.GetAndCreateWriteCompletionFunc(shared_socket->SockFd());
        shared_completion->action = CompletionQueue::ACTION_WRITE;
        shared_completion->write_func = std::forward<HandlerType>(handler);
        shared_completion->buffer = std::forward<StringType>(buffer);
        shared_completion->shared_socket = shared_socket;
        return true;
    }

    ////////////////////////////////////////////////////////////
    // Async Recv
    LSF_FUNC_ALIAS(AsyncRead, AsycRecv)

    template <typename HandlerType1, typename HandlerType2 = std::nullptr_t>
    bool AsyncRead(SharedSocket shared_socket, HandlerType1&& handler, HandlerType2&& peer_close_handler = nullptr) {
        // register event
        if (!_pdriver->RegisterEvent(shared_socket->SockFd(), BasicEventDriver::FLAG_READ)) {
            SetErrString(LSF_DEBUG_INFO + _pdriver->ErrString());
            return false;
        }

        // add completion task
        auto shared_completion = _queue.GetAndCreateReadCompletionFunc(shared_socket->SockFd());
        shared_completion->action = CompletionQueue::ACTION_READ;
        shared_completion->read_func = std::forward<HandlerType1>(handler);
        shared_completion->read_peer_close_func = std::forward<HandlerType2>(peer_close_handler);
        shared_completion->shared_socket = shared_socket;
        return true;
    }

    ////////////////////////////////////////////////////////////
    // Async Raw Callback
    template<typename HandlerType>
    bool AsyncRawCallback(int sockfd, int flags, HandlerType&& handler) {
        // register event
        if (!_pdriver->RegisterEvent(sockfd, flags)) {
            SetErrString(LSF_DEBUG_INFO + _pdriver->ErrString());
            return false;
        }

        // add completion task
        auto shared_completion = _queue.GetAndCreateRawCompletionFunc(sockfd);
        shared_completion->raw_func = std::forward<HandlerType>(handler);
        return true;
    }

    ////////////////////////////////////////////////////////////
    // Async Timer
    template <typename HandlerType>
    int AsyncAddTimerOnce(uint64_t milli_expire, HandlerType&& handler) {
        return AsyncAddTimer(milli_expire, 0, 1, std::forward<HandlerType>(handler));
    }

    template <typename HandlerType>
    int AsyncAddTimerForever(uint64_t milli_expire, HandlerType&& handler) {
        return AsyncAddTimer(milli_expire, milli_expire, 0, std::forward<HandlerType>(handler));
    }

    template <typename HandlerType>
    int AsyncAddTimerForeverAndTriggerNow(uint64_t milli_expire, HandlerType&& handler) {
        return AsyncAddTimer(1, milli_expire, 0, std::forward<HandlerType>(handler));
    }

    template <typename HandlerType>
    int AsyncAddTimerMulti(uint64_t milli_expire, size_t trigger_count, HandlerType&& handler) {
        return AsyncAddTimer(milli_expire, milli_expire, trigger_count, std::forward<HandlerType>(handler));
    }

    template <typename HandlerType>
    int AsyncAddTimer(uint64_t milli_expire, uint64_t milli_interval, size_t trigger_count, HandlerType&& handler) {
        // check input
        if (milli_expire == 0) milli_expire = 1;

        // create timer
        int fd = ErrWrap(::timerfd_create(CLOCK_REALTIME, TFD_NONBLOCK));
        if (fd < 0) return -1;

        // set timeout
        itimerspec timer_info;
        timer_info.it_value.tv_sec = milli_expire / 1000;
        timer_info.it_value.tv_nsec = (milli_expire % 1000) * 1000000;
        timer_info.it_interval.tv_sec = milli_interval / 1000;
        timer_info.it_interval.tv_nsec = (milli_interval % 1000) * 1000000;
        if (ErrWrap(::timerfd_settime(fd, 0, &timer_info, nullptr)) < 0) {
            ::close(fd);
            return -1;
        }

        // register event
        if (!_pdriver->RegisterEvent(fd, BasicEventDriver::FLAG_READ)) {
            ::close(fd);
            SetErrString(LSF_DEBUG_INFO + _pdriver->ErrString());
            return -1;
        }

        // add completion task
        auto shared_completion = _queue.GetAndCreateReadCompletionFunc(fd);
        shared_completion->action = CompletionQueue::ACTION_TIMER;
        shared_completion->timer_func = std::forward<HandlerType>(handler);
        shared_completion->timer_count = trigger_count;
        shared_completion->timer_fd = fd;
        return fd;
    }

    std::pair<uint64_t,uint64_t> AsyncGetTimer(int fd) {
        // get timer
        itimerspec timer_info;
        if (ErrWrap(::timerfd_gettime(fd, &timer_info)) < 0) return std::make_pair(0,0);

        uint64_t expire = timer_info.it_value.tv_sec * 1000 + timer_info.it_value.tv_nsec / 1000;
        uint64_t interval = timer_info.it_interval.tv_sec * 1000 + timer_info.it_interval.tv_nsec / 1000;
        return std::make_pair(expire, interval);
    }

    ////////////////////////////////////////////////////////////
    // Async Close
public:
    void AsyncCancel(SharedSocket socket) {
        if (socket) AsyncCancel(socket->SockFd());
    }

    void AsyncCancel(int fd) {
        // cancel registration
        _pdriver->CancelEvent(fd);

        // cancel completion task
        _queue.CancelReadCompletionFunc(fd);
        _queue.CancelWriteCompletionFunc(fd);
        _queue.CancelRawCompletionFunc(fd);

        // clear unused data
        ReadDataBuffer::Instance()->ClearUnusedData(fd);
    }

    void AsyncCloseTimer(int sockfd) {
        AsyncCancel(sockfd);
        ::close(sockfd);
    }

    ////////////////////////////////////////////////////////////
    // Main Routine
    void Run() {
        while (true) {
            int fd, flag;

            // update clock
            UpdateClock();

            // process tick function
            for (auto& func : _tick_funcs) func();

            // wait events
            _pdriver->WaitEvent(MAX_WAIT_MILLI_SECONDS);

            // traverse ready events
            while (_pdriver->GetReadyEvent(&fd, &flag)) {
                // if register raw completion, just callback
                auto raw_completion = _queue.GetRawCompletionFunc(fd);
                if (raw_completion) {
                    if (raw_completion->raw_func) raw_completion->raw_func(fd, flag);
                    continue;
                }

                // read action
                if (flag & BasicEventDriver::FLAG_READ) {
                    auto read_completion = _queue.GetReadCompletionFunc(fd);
                    if (read_completion) {
                        switch (read_completion->action) {
                            case CompletionQueue::ACTION_ACCEPT: OnAcceptEvent(read_completion); break;
                            case CompletionQueue::ACTION_READ:   OnReadEvent(read_completion);   break;
                            case CompletionQueue::ACTION_TIMER:  OnTimerEvent(read_completion);  break;
                            default: break;
                        }
                    }
                    else {
                        LSF_LOG_FAT("cant get read handler but event triggers, fd=%u", fd);
                        AsyncCancel(fd);
                    }
                }

                // write action
                if (flag & BasicEventDriver::FLAG_WRITE) {
                    auto write_completion = _queue.GetWriteCompletionFunc(fd);
                    if (write_completion) {
                        switch (write_completion->action) {
                            case CompletionQueue::ACTION_WRITE:   OnWriteEvent(write_completion);   break;
                            case CompletionQueue::ACTION_CONNECT: OnConnectEvent(write_completion); break;
                            default: break;
                        }
                    }
                    else {
                        LSF_LOG_FAT("cant get write handler buf event triggers, fd=%u", fd);
                        AsyncCancel(fd);
                    }
                }

                // error handle
                if (flag & BasicEventDriver::FLAG_ERR) {
                    OnErrorEvent(fd);
                }
            }

            // check exit
            if (_is_exit) {
                _is_exit = false;
                break;
            }
        }
    }

    ////////////////////////////////////////////////////////////
    // Accept Event
    void OnAcceptEvent(CompletionQueue::shared_read_completion_type shared_completion) {
        // accept
        SharedSocket shared_accept_socket(shared_completion->shared_socket->Accept());
        if (!shared_accept_socket) {
            SetErrString(LSF_DEBUG_INFO + shared_completion->shared_socket->ErrString());
            if (shared_completion->accept_fail_func) shared_completion->accept_fail_func(shared_completion->shared_socket);
            return;
        }

        // call accept handler
        if (shared_completion->accept_func) shared_completion->accept_func(shared_completion->shared_socket, shared_accept_socket);
    }

    ////////////////////////////////////////////////////////////
    // Connect Event
    void OnConnectEvent(CompletionQueue::shared_write_completion_type shared_completion) {
        // check ret
        if (!shared_completion->shared_socket->CheckSockError()) {
            SetErrString(LSF_DEBUG_INFO + shared_completion->shared_socket->ErrString());
            if (shared_completion->connect_fail_func) shared_completion->connect_fail_func(shared_completion->shared_socket, shared_completion->sockaddr);
            AsyncCancel(shared_completion->shared_socket);
            return;
        }

        // always cancel write registration after trigger, must do before callback
        AsyncCancel(shared_completion->shared_socket->SockFd());

        // set connect status
        shared_completion->shared_socket->_is_connect = true;

        // call write handler
        if (shared_completion->connect_func)
            shared_completion->connect_func(shared_completion->shared_socket, shared_completion->sockaddr);
    }

    ////////////////////////////////////////////////////////////
    // Write Event
    void OnWriteEvent(CompletionQueue::shared_write_completion_type shared_completion) {
        // write data
        size_t total = 0;
        auto& shared_socket = shared_completion->shared_socket;
        auto& buffer = shared_completion->buffer;
        while (total < shared_completion->buffer.length()) {
            // try send
            int ret = ::send(shared_socket->SockFd(), buffer.data()+total, buffer.length()-total, 0);

            // handle error
            if (ret < 0) {
                if (errno == EINTR || errno == EAGAIN || errno == EWOULDBLOCK) { // signal interrupt or not ready
                    continue;
                }
                else { // error
                    AsyncCancel(shared_socket);
                    shared_socket->ShutDown();
                    return;
                }
            }

            // handle send data
            total += ret;
        }

        // call write handler
        if (shared_completion->write_func)
            shared_completion->write_func(shared_socket, shared_completion->buffer);

        // always cancel write registration after event
        AsyncCancel(shared_socket->SockFd());
    }

    ////////////////////////////////////////////////////////////
    // Read Event
    void OnReadEvent(CompletionQueue::shared_read_completion_type shared_completion) {
        // first check and append unused data
        auto& shared_socket = shared_completion->shared_socket;
        std::string buffer;
        ReadDataBuffer::Instance()->CheckUnusedData(shared_socket->SockFd(), buffer);

        // read loop
        bool shutdown_socket = false;
        bool peer_close = false;
        while (true) {
            char tmp[MAX_BUFFER_LEN];
            ssize_t ret = ::recv(shared_socket->SockFd(), tmp, sizeof(tmp), 0);

            // handle error
            if (ret < 0) {
                if (errno == EINTR) { // signal interrupt
                    continue;
                } else if (errno == EAGAIN || errno == EWOULDBLOCK) { // no data
                    break;
                } else { // error
                    shutdown_socket = true;
                    break;
                }
            }

            // handle shared_socket close
            if (ret == 0) {
                peer_close = true;
                shutdown_socket = true;
                break;
            }

            // handle read data
            buffer.append(tmp, ret);
        }

        // handle read first
        if (!buffer.empty() && shared_completion->read_func) shared_completion->read_func(shared_socket, buffer);

        // handle shutdown connection
        if (shutdown_socket) {
            AsyncCancel(shared_socket);
            shared_socket->ShutDown();
        }

        // handle peer close
        if (peer_close && shared_completion->read_peer_close_func) shared_completion->read_peer_close_func(shared_socket);

    }

    ////////////////////////////////////////////////////////////
    // Timer Event
    void OnTimerEvent(CompletionQueue::shared_read_completion_type shared_completion) {
        // read data
        uint64_t count;
        int timer_fd = shared_completion->timer_fd;
        int ret = ::read(timer_fd, &count, sizeof(count));
        if (ret != sizeof(count)) { // return error
            // not block reason then close
            if (ret != EAGAIN) AsyncCloseTimer(timer_fd);
            return;
        }

        // call write handler as many as it triggers
        bool should_delete = false;
        for (uint64_t i = 0; i < count; ++i) {
            // callback
            if (shared_completion->timer_func) shared_completion->timer_func(timer_fd);

            // check remain count
            if (shared_completion->timer_count == 1) {
                should_delete = true;
                break;
            }
            else if (shared_completion->timer_count != 0) {
                shared_completion->timer_count--;
            }
        }

        // check if need delete
        if (should_delete) {
            AsyncCloseTimer(timer_fd);
            return;
        }
        if (AsyncGetTimer(timer_fd).second == 0) {
            AsyncCloseTimer(timer_fd);
            return;
        }
    }

    ////////////////////////////////////////////////////////////
    // Error Event
    void OnErrorEvent(int fd) {
        // check read completion
        auto shared_read_completion = _queue.GetReadCompletionFunc(fd);
        if (shared_read_completion) {
            if (shared_read_completion->action == CompletionQueue::ACTION_TIMER) {
                // just release timer
                AsyncCloseTimer(fd);
            }
            else {
                // shutdown shared_socket
                AsyncCancel(shared_read_completion->shared_socket);
                shared_read_completion->shared_socket->ShutDown();
            }
        }

        // check write completion
        auto shared_write_completion = _queue.GetWriteCompletionFunc(fd);
        if (shared_write_completion) {
            // check connect fail action
            if (shared_write_completion->action == CompletionQueue::ACTION_CONNECT) {
                shared_write_completion->shared_socket->CheckSockError();
                SetErrString(LSF_DEBUG_INFO + shared_write_completion->shared_socket->ErrString());
                if (shared_write_completion->connect_fail_func)
                    shared_write_completion->connect_fail_func(shared_write_completion->shared_socket, shared_write_completion->sockaddr);
            }

            // shutdown
            AsyncCancel(shared_write_completion->shared_socket);
            shared_write_completion->shared_socket->ShutDown();
        }
    }

    ////////////////////////////////////////////////////////////
    // Other Func
    template<typename FuncType>
    void RegisterTickFunction(FuncType&& func) {
        _tick_funcs.push_back(std::forward<FuncType>(func));
    }

    template<typename SharedSocket>
    void SaveUnusedData(SharedSocket shared_socket, std::string const& data) {
        ReadDataBuffer::Instance()->AppendUnusedData(shared_socket->SockFd(), data);
    }
    template<typename SharedSocket>
    void SaveUnusedData(SharedSocket shared_socket, char const* data, size_t len) {
        ReadDataBuffer::Instance()->AppendUnusedData(shared_socket->SockFd(), data, len);
    }

    void SetExit() { _is_exit = true; }

    uint64_t ClockTime() const { return _ts.tv_sec + _ts.tv_nsec / 1000000000; }
    uint64_t ClockTimeMilli() const { return _ts.tv_sec * 1000 + _ts.tv_nsec / 1000000; }
    uint64_t ClockTimeMicro() const { return _ts.tv_sec * 1000000 + _ts.tv_nsec / 1000; }
    uint64_t ClockTimeNano() const { return _ts.tv_sec * 1000000000 + _ts.tv_nsec; }
    uint64_t RealClockTime() { UpdateClock(); return ClockTime(); }
    uint64_t RealClockTimeMilli() { UpdateClock(); return ClockTimeMilli(); }
    uint64_t RealClockTimeMicro() { UpdateClock(); return ClockTimeMicro(); }
    uint64_t RealClockTimeNano() { UpdateClock(); return ClockTimeNano(); }

    BasicEventDriver const* Driver() const { return _pdriver; }
    CompletionQueue const& Queue() const { return _queue; }

private:
    void UpdateClock() { ::clock_gettime(CLOCK_REALTIME, &_ts); }

private:
    BasicEventDriver*                  _pdriver = nullptr;
    bool                               _is_exit = false;
    timespec                           _ts      = {0,0};
    CompletionQueue                    _queue;
    std::vector<std::function<void()>> _tick_funcs;
};

}  // end of namespace async
}  // end of namespace asio
}  // end of namespace lsf

// vim:ts=4:sw=4:et:ft=cpp:
