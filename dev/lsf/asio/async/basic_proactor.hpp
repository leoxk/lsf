// File:        basic_proactor.hpp
// Description: ---
// Notes:       ---
// Author:      leoxiang <leoxiang727@qq.com>
// Revision:    2012-06-12 by leoxiang

#pragma once

#include <sys/timerfd.h>
#include <sys/time.h>
#include <string>
#include "lsf/basic/noncopyable.hpp"
#include "lsf/basic/error.hpp"
#include "lsf/basic/macro.hpp"
#include "lsf/basic/buffer.hpp"
#include "lsf/asio/detail/basic_socket.hpp"
#include "lsf/asio/detail/basic_listen_socket.hpp"
#include "lsf/asio/async/completion_queue.hpp"
#include "lsf/asio/async/epoll_event.hpp"
#include "lsf/asio/async/poll_event.hpp"

namespace lsf {
namespace asio {
namespace async {

////////////////////////////////////////////////////////////
// BasicProactorSerivce
////////////////////////////////////////////////////////////
template<typename EventDriver>
class BasicProactorSerivce : 
    public basic::NonCopyable,
    public basic::Error
{
public:
    typedef EventDriver             driver_type;
    typedef std::function<bool()>   routine_func_type;

    static const size_t MAX_WAIT_MILLI_SECONDS = 10;
    static const size_t MAX_BUFFER_LEN = 128 * 1024;

public:
    BasicProactorSerivce() : _is_exit(false) { }

    ////////////////////////////////////////////////////////////
    // Async Accept
    template<typename SocketType, typename HandlerType>
    bool AsyncAccept(SocketType & socket, HandlerType const & handler)
    {
        // set non block
        if (!socket.SetNonBlock())
        {
            ErrString() = LSF_DEBUG_INFO + socket.ErrString(); 
            return false;
        }

        // register event
        if (!_driver.RegisterEvent(socket.GetSockFd(), driver_type::FLAG_READ))
        {
            ErrString() = LSF_DEBUG_INFO + _driver.ErrString();
            return false;
        }

        // add completion task
        if (!_queue.AddCompletionTask(socket.GetSockFd(), CompletionFunc::ACTION_ACCEPT, handler))
        {
            _driver.CancelEvent(socket.GetSockFd());
            ErrString() = LSF_DEBUG_INFO + _queue.ErrString();
            return false;
        }

        return true;
    }

    ////////////////////////////////////////////////////////////
    // Async Connect
    template<typename SocketType, typename SockAddrType, typename HandlerType>
    bool AsyncConnect(SocketType & socket, SockAddrType const & sockaddr, HandlerType const & handler)
    {
        // set non block
        if (!socket.SetNonBlock())
        {
            ErrString() = LSF_DEBUG_INFO + socket.ErrString(); 
            return false;
        }

        // try connect
        if (!socket.Connect(sockaddr) && errno != EINPROGRESS)
        {
            ErrString() = LSF_DEBUG_INFO + socket.ErrString(); 
            return false;
        }

        // register event
        if (!_driver.RegisterEvent(socket.GetSockFd(), driver_type::FLAG_WRITE))
        {
            ErrString() = LSF_DEBUG_INFO + _driver.ErrString();
            return false;
        }

        // add completion task
        if (!_queue.AddCompletionTask(socket.GetSockFd(), CompletionFunc::ACTION_CONNECT, handler))
        {
            _driver.CancelEvent(socket.GetSockFd());
            ErrString() = LSF_DEBUG_INFO + _queue.ErrString();
            return false;
        }

        return true;
    }

    ////////////////////////////////////////////////////////////
    // Async Read
    template<typename SocketType, typename HandlerType1, typename HandlerType2>
    bool AsyncRead(SocketType & socket, HandlerType1 const & handler, HandlerType2 const & peer_close_handler)
    {
        // call same routine
        if (!AsyncRead(socket, handler)) return false;

        // add peer close handler
        if (!_queue.AddCompletionTask(socket.GetSockFd(), CompletionFunc::ACTION_PEER_CLOSE, peer_close_handler))
        {
            _driver.CancelEvent(socket.GetSockFd());
            ErrString() = LSF_DEBUG_INFO + _queue.ErrString();
            return false;
        }

        return true;
    }

    template<typename SocketType, typename HandlerType>
    bool AsyncRead(SocketType & socket, HandlerType const & handler)
    {
        // set non block
        if (!socket.SetNonBlock())
        {
            ErrString() = LSF_DEBUG_INFO + socket.ErrString(); 
            return false;
        }

        // register event
        if (!_driver.RegisterEvent(socket.GetSockFd(), driver_type::FLAG_READ))
        {
            ErrString() = LSF_DEBUG_INFO + _driver.ErrString();
            return false;
        }

        // add completion task
        if (!_queue.AddCompletionTask(socket.GetSockFd(), CompletionFunc::ACTION_READ, handler))
        {
            _driver.CancelEvent(socket.GetSockFd());
            ErrString() = LSF_DEBUG_INFO + _queue.ErrString();
            return false;
        }

        return true;
    }

    ////////////////////////////////////////////////////////////
    // Async Write
    template<typename SocketType, typename HandlerType>
    bool AsyncWrite(SocketType & socket, std::string const & buffer, HandlerType const & handler)
    {
        return AsyncWrite(socket, buffer.c_str(), buffer.length(), handler);
    }

    template<typename SocketType, typename HandlerType>
    bool AsyncWrite(SocketType & socket, void const * buffer, size_t buflen, HandlerType const & handler)
    {
        // set non block
        if (!socket.SetNonBlock())
        {
            ErrString() = LSF_DEBUG_INFO + socket.ErrString(); 
            return false;
        }

        // register event
        if (!_driver.RegisterEvent(socket.GetSockFd(), driver_type::FLAG_WRITE))
        {
            ErrString() = LSF_DEBUG_INFO + _driver.ErrString();
            return false;
        }

        // add completion task
        if (!_queue.AddCompletionTask(socket.GetSockFd(), CompletionFunc::ACTION_WRITE, handler, buffer, buflen))
        {
            _driver.CancelEvent(socket.GetSockFd());
            ErrString() = LSF_DEBUG_INFO + _queue.ErrString();
            return false;
        }

        return true;
    }

    ////////////////////////////////////////////////////////////
    // Async Timer
    template<typename HandlerType>
    bool AsyncAddTimer(uint64_t milli_expire, uint64_t milli_interval, HandlerType const & handler, int * pfd = NULL)
    {
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
        if (ErrWrap(::timerfd_settime(fd, 0, &timer_info, NULL)) < 0)
        {
            ::close(fd);
            return false;
        }

        // register event
        if (!_driver.RegisterEvent(fd, driver_type::FLAG_READ))
        {
            ::close(fd);
            ErrString() = LSF_DEBUG_INFO + _driver.ErrString();
            return false;
        }

        // add completion task
        if (!_queue.AddCompletionTask(fd, CompletionFunc::ACTION_TIMER, handler))
        {
            _driver.CancelEvent(fd);
            ::close(fd);
            ErrString() = LSF_DEBUG_INFO + _queue.ErrString();
            return false;
        }

        if (pfd != NULL) *pfd = fd;
        return true;
    }

    void AsyncDelTimer(int fd)
    {
        // clear timer
        itimerspec timer_info;
        memset(&timer_info, 0, sizeof(timer_info));
        ::timerfd_settime(fd, 0, &timer_info, NULL);

        // clear async
        CloseAsync(fd);
        ::close(fd);
    }

    bool AsyncGetTimer(int fd, uint64_t * p_expire, uint64_t * p_interval = NULL)
    {
        // get timer
        itimerspec timer_info;
        if (ErrWrap(::timerfd_gettime(fd, &timer_info)) < 0) return false;

        if (p_expire) *p_expire = timer_info.it_value.tv_sec * 1000 + timer_info.it_value.tv_nsec / 1000;
        if (p_interval) *p_interval = timer_info.it_interval.tv_sec * 1000 + timer_info.it_interval.tv_nsec / 1000;
        return true;
    }

    ////////////////////////////////////////////////////////////
    // Close Async
    void CloseAsync(int fd)
    {
        // cancel registration
        _driver.CancelEvent(fd);
        
        // cancel completion task
        _queue.CancelCompletionTask(fd);
    }

    ////////////////////////////////////////////////////////////
    // Main Routine
    void Run()
    {
        while (true)
        {
            int fd, flag;

            // update clock
            _UpdateClock();

            // wait events
            _driver.WaitEvent(MAX_WAIT_MILLI_SECONDS);

            // traverse ready events
            while (_driver.GetReadyEvent(&fd, &flag))
            {
                // read action
                if (flag & driver_type::FLAG_READ)
                {
                    CompletionFunc * pfunc = NULL;
                    if (!_queue.GetReadCompletionTask(fd, &pfunc)) continue;
                    
                    switch (pfunc->action)
                    {
                        case CompletionFunc::ACTION_ACCEPT: OnAcceptEvent(fd); break;
                        case CompletionFunc::ACTION_READ  : OnReadEvent(fd); break;
                        case CompletionFunc::ACTION_TIMER : OnTimerEvent(fd); break;
                        default: continue;
                    }
                }

                // write action
                if (flag & driver_type::FLAG_WRITE)
                {
                    CompletionFunc * pfunc = NULL;
                    if (!_queue.GetWriteCompletionTask(fd, &pfunc)) continue;
                    
                    switch (pfunc->action)
                    {
                        case CompletionFunc::ACTION_WRITE: OnWriteEvent(fd); break;
                        case CompletionFunc::ACTION_CONNECT: OnConnectEvent(fd); break;
                        default: continue;
                    }
                }

                // error handle
                if (flag & driver_type::FLAG_ERR)
                {
                    CloseAsync(fd);
                    ::close(fd);
                }
            }

            // call routine 
            if (_routine_func && _routine_func())
            {
                _is_exit = true; 
            }

            // check exit
            if (_is_exit) break;
        }

    }

    ////////////////////////////////////////////////////////////
    // Accept Event
    void OnAcceptEvent(int fd)
    {
        detail::BasicListenSocket<detail::DummyProtocol> listen_socket(fd);
        static AsyncInfo info;
        info.Clear();
        info.fd = fd;

        // accept
        detail::BasicSocket<detail::DummyProtocol> accept_socket;
        if (!listen_socket.Accept(accept_socket))
        {
            ErrString() = LSF_DEBUG_INFO + listen_socket.ErrString();
            return;
        }
        info.accept_fd = accept_socket.GetSockFd();

        // call accept handler
        CompletionFunc * pfunc = NULL;
        if (!_queue.GetReadCompletionTask(fd, &pfunc)) return;
        pfunc->func(info);
    }

    ////////////////////////////////////////////////////////////
    // Read Event
    void OnReadEvent(int fd)
    {
        detail::BasicSocket<detail::DummyProtocol> socket(fd);
        static AsyncInfo info;
        info.Clear();
        info.fd = fd;
        info.buffer.reserve(MAX_BUFFER_LEN); // avoid copy

        // read loop
        bool close_connection = false;
        bool peer_close = false;
        while (true)
        {
            static char buffer[MAX_BUFFER_LEN];
            ssize_t ret = socket.Recv(buffer, sizeof(buffer));

            // handle error
            if (ret < 0)
            {
                if (errno == EINTR) // signal interrupt
                {
                    continue;
                }
                else if (errno == EAGAIN || errno == EWOULDBLOCK)   // no data
                {
                    break;
                }
                else // error
                {
                    close_connection = true;
                    break;
                }
            }

            // handle socket close
            if (ret == 0)
            {
                peer_close = true;
                close_connection = true;
                break;
            }
            
            // handle read data
            info.buffer.append(buffer, ret);
        }
        
        // call read handler first
        if (!info.buffer.empty())
        {
            CompletionFunc * pfunc = NULL;
            if (_queue.GetReadCompletionTask(fd, &pfunc))
            {
                // set close if necessary
                if (!pfunc->func(info))
                {
                    close_connection = true;
                }
            }
        }

        // call peer close handler
        if (peer_close)
        {
            CompletionFunc * pfunc = NULL;
            if (_queue.GetPeerCloseCompletionTask(fd, &pfunc))
            {
                pfunc->func(info);
            }
            CloseAsync(fd);
            ::close(fd);
            return;
        }

        // check close connection
        if (close_connection)
        {
            CloseAsync(fd);
            ::close(fd);
        }
    }

    ////////////////////////////////////////////////////////////
    // Write Event
    void OnWriteEvent(int fd)
    {
        detail::BasicSocket<detail::DummyProtocol> socket(fd);
        static AsyncInfo info;
        info.Clear();
        info.fd = fd;

        // get completion func
        CompletionFunc * pfunc = NULL;
        if (!_queue.GetWriteCompletionTask(fd, &pfunc))
        {
            CloseAsync(fd);
            ::close(fd);
            return;
        }

        // write data
        size_t total = 0;
        bool close_connection = false;
        while (total < pfunc->buffer.length())
        {
            int ret = socket.Send(pfunc->buffer.data() + total, pfunc->buffer.length() - total);

            // handle error
            if (ret < 0)
            {
                if (errno == EINTR || errno == EAGAIN || errno == EWOULDBLOCK) // signal interrupt or not ready
                {
                    continue;
                }
                else // error
                {
                    close_connection = true;
                    break;
                }
            }

            // handle send data
            total += ret;
        }

        // if send wrong
        if (close_connection)
        {
            CloseAsync(fd);
            ::close(fd);
        }
        // call write handler
        else if (!pfunc->func(info))
        {
            CloseAsync(fd);
            ::close(fd);
        }
        else
        {
            // always cancel write registration after event
            _driver.CancelEvent(fd);
            _queue.CancelCompletionTask(fd);
        }
    }

    ////////////////////////////////////////////////////////////
    // Connect Event
    void OnConnectEvent(int fd)
    {
        detail::BasicSocket<detail::DummyProtocol> socket(fd);
        static AsyncInfo info;
        info.Clear();
        info.fd = fd;

        // get completion func
        CompletionFunc * pfunc = NULL;
        if (!_queue.GetWriteCompletionTask(fd, &pfunc))
        {
            CloseAsync(fd);
            ::close(fd);
            return;
        }

        // call write handler
        if (!pfunc->func(info))
        {
            CloseAsync(fd);
            ::close(fd);
        }
        else
        {
            // always cancel write registration after event
            CloseAsync(fd);
        }
    }

    ////////////////////////////////////////////////////////////
    // Timer Event
    void OnTimerEvent(int fd)
    {
        static AsyncInfo info;
        info.Clear();
        info.fd = fd;

        // read data
        uint64_t count;
        int ret = ::read(fd, &count, sizeof(count));
        if (ret != sizeof(count))
        {
            if (ret != EAGAIN) AsyncDelTimer(fd);
            return;
        }

        // get completion func
        CompletionFunc * pfunc = NULL;
        if (!_queue.GetReadCompletionTask(fd, &pfunc))
        {
            AsyncDelTimer(fd);
            return;
        }

        // call write handler as many as it triggers
        for (uint64_t i = 0; i < count; ++i)
        {
            if (!pfunc->func(info))
            {
                AsyncDelTimer(fd);
                return;
            }
        }

        // check if need delete
        uint64_t expire;
        if (!AsyncGetTimer(fd, &expire))
        {
            AsyncDelTimer(fd);
            return;
        }
        if (expire == 0)
        {
            AsyncDelTimer(fd);
        }
    }

    ////////////////////////////////////////////////////////////
    // Other Func
    template<typename HandlerType>
    void SetRoutineFunc(HandlerType const & handler)
    {
        _routine_func = routine_func_type(handler);
    }

    void SetExit() { _is_exit = true; }

    void GetClockTime() const { return _ts.tv_sec + _ts.tv_nsec / 1000000000; }
    void GetClockTimeMilli() const { return _ts.tv_sec * 1000 + _ts.tv_nsec / 1000000; }
    void GetClockTimeMicro() const { return _ts.tv_sec * 1000000 + _ts.tv_nsec / 1000; }
    void GetClockTimeNano() const { return _ts.tv_sec * 1000000000 + _ts.tv_nsec; }

private:
    void _UpdateClock() { ::clock_gettime(CLOCK_REALTIME, &_ts); }

private:
    driver_type         _driver;
    CompletionQueue     _queue;
    bool                _is_exit;
    routine_func_type   _routine_func;
    timespec            _ts;
};

} // end of namespace detail
} // end of namespace asio
} // end of namespace lsf

// vim:ts=4:sw=4:et:ft=cpp:
