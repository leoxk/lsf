// File:        basic_proactor.hpp
// Description: ---
// Notes:       ---
// Author:      leoxiang <leoxiang727@qq.com>
// Revision:    2012-06-12 by leoxiang

#pragma once

#include <string>
#include "lsf/basic/noncopyable.hpp"
#include "lsf/basic/error.hpp"
#include "lsf/basic/macro.hpp"
#include "lsf/basic/buffer.hpp"
#include "lsf/asio/detail/basic_socket.hpp"
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
    typedef EventDriver     driver_type;

    static const size_t MAX_WAIT_MILLI_SECONDS = 10;
    static const size_t MAX_BUFFER_LEN = 128 * 1024;

public:
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
            ErrString() = LSF_DEBUG_INFO + _queue.ErrString();
            return false;
        }

        return true;
    }

    ////////////////////////////////////////////////////////////
    // Async Write
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
            ErrString() = LSF_DEBUG_INFO + _queue.ErrString();
            return false;
        }

        return true;
    }

    template<typename SocketType, typename HandlerType>
    bool AsyncWrite(SocketType & socket, std::string const & buffer, HandlerType const & handler)
    {
        return AsyncWrite(socket, buffer.c_str(), buffer.length(), handler);
    }

    ////////////////////////////////////////////////////////////
    // Async Timer
    bool AsyncTimer()
    {
        // TODO
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

        // close socket
        ::close(fd);
    }

    ////////////////////////////////////////////////////////////
    // Main Routine
    void Run()
    {
        while (true)
        {
            int fd, flag;

            // wait events
            if (!_driver.WaitEvent(MAX_WAIT_MILLI_SECONDS)) continue;

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
                }
            }
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
        if (!pfunc->func(info)) CloseAsync(fd);
    }

    ////////////////////////////////////////////////////////////
    // Read Event
    void OnReadEvent(int fd)
    {
        detail::BasicSocket<detail::DummyProtocol> socket(fd);
        static AsyncInfo info;
        info.Clear();
        info.fd = fd;

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
                else if ( errno == EAGAIN || errno == EWOULDBLOCK)   // no data
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
            return;
        }

        // check close connection
        if (close_connection) CloseAsync(fd);
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
            return;
        }

        // write data
        size_t total = 0;
        bool close_connection = false;
        while (true)
        {
            int ret = socket.Send(pfunc->buffer.data() + total, pfunc->buffer.length() - total);

            // handle error
            if (ret < 0)
            {
                if (errno == EINTR) // signal interrupt
                {
                    continue;
                }
                else if (errno == EAGAIN || errno == EWOULDBLOCK) // not ready
                {
                    break;
                }
                else // error
                {
                    close_connection = true;
                    break;
                }
            }

            // handle send data
            total += ret;
            if (total >= pfunc->buffer.length()) break;
        }

        // if send wrong
        if (close_connection)
        {
            CloseAsync(fd);
        }
        // call write handler
        else if (!pfunc->func(info))
        {
            CloseAsync(fd);
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
            return;
        }

        // call write handler
        if (!pfunc->func(info))
        {
            CloseAsync(fd);
        }
        else
        {
            // always cancel write registration after event
            _driver.CancelEvent(fd);
            _queue.CancelCompletionTask(fd);
        }
    }

    ////////////////////////////////////////////////////////////
    // Timer Event
    void OnTimerEvent(int fd)
    {
        // TODO
    }

private:
    driver_type     _driver;
    CompletionQueue _queue;
};

} // end of namespace detail
} // end of namespace asio
} // end of namespace lsf

// vim:ts=4:sw=4:et:ft=cpp:
