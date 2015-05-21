// File:        basic_proactor.hpp
// Description: ---
// Notes:       ---
// Author:      leoxiang <leoxiang727@qq.com>
// Revision:    2012-06-12 by leoxiang

#pragma once

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
    // Async Funcs
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
        if (!_driver.RegisterEvent(socket.GetSockFd(), driver_type::FLAG_READ | driver_type::FLAG_ERR))
        {
            ErrString() = LSF_DEBUG_INFO + _driver.ErrString();
            return false;
        }

        // add completion task
        if (!_queue.AddCompletionTask(socket.GetSockFd(), EN_COMPLETION_TYPE_ACCEPT, handler))
        {
            _driver.CancelEvent(socket.GetSockFd());
            ErrString() = LSF_DEBUG_INFO + _queue.ErrString();
            return false;
        }

        return true;
    }

    bool AsyncConnect()
    {
        return true;
    }

    bool AsyncRead();
    bool AsyncWrite();
    bool AsyncTimer();

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
                // get completion task
                CompletionFunc * pfunc = NULL;
                if (!_queue.GetReadCompletionTask(fd, &pfunc)) continue;

                // decide read action
                if (flag & driver_type::FLAG_READ)
                {
                    switch (pfunc->type)
                    {
                        case EN_COMPLETION_TYPE_ACCEPT: OnAcceptEvent(fd); break;
                        case EN_COMPLETION_TYPE_READ  : OnReadEvent(fd); break;
                        case EN_COMPLETION_TYPE_TIMER : OnTimerEvent(fd); break;
                        default: continue;
                    }
                }

                // decide write action
                if (flag & driver_type::FLAG_WRITE)
                {
                    OnWriteEvent(fd);
                }

                // error handle
                if (flag & driver_type::FLAG_ERR)
                {
                    CloseAsync(fd);
                }
            }
        }

    }

    void OnAcceptEvent(int fd)
    {

    }

    void OnReadEvent(int fd)
    {
        // read data
        static basic::Buffer<MAX_BUFFER_LEN>  buffer;
        //buffer.SetSize(0);
        detail::BasicSocket<detail::DummyProtocol> socket(fd);

        // read loop
        while (true)
        {
            ssize_t size = socket.Recv(buffer.Data(), buffer.MaxSize());

            // handle error
            if (size < 0)
            {
                if (errno == EINTR) // signal interrupt
                {
                    continue;
                }
                else if ( errno == EAGAIN || errno == EWOULDBLOCK)   // no data
                {
                    break;
                }
                else
                {
                    CloseAsync(fd);
                    break;
                }
            }

            // handle socket close
            if (size == 0)
            {
                CloseAsync(fd);
                break;
            }
            
            // handle read data
            break;
        }
        
        // TODO 

        // call register handler
        CompletionFunc * pfunc = NULL;
        if (!_queue.GetReadCompletionTask(fd, &pfunc)) return;
        pfunc->func();
    }

    void OnTimerEvent(int fd)
    {

    }

    void OnWriteEvent(int fd)
    {
        CompletionFunc * pfunc = NULL;
        if (!_queue.GetWriteCompletionTask(fd, &pfunc)) return;
        pfunc->func();
    }

private:
    driver_type     _driver;
    CompletionQueue _queue;
};

} // end of namespace detail
} // end of namespace asio
} // end of namespace lsf

// vim:ts=4:sw=4:et:ft=cpp:
