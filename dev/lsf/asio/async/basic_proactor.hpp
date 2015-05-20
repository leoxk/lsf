// File:        basic_proactor.hpp
// Description: ---
// Notes:       ---
// Author:      leoxiang <leoxiang727@qq.com>
// Revision:    2012-06-12 by leoxiang

#pragma once

#include "lsf/basic/noncopyable.hpp"
#include "lsf/basic/error.hpp"
#include "lsf/basic/macro.hpp"
#include "lsf/asio/detail/basic_socket.hpp"
#include "lsf/asio/async/completion_queue.hpp"
#include "lsf/asio/async/epoll_event.hpp"
#include "lsf/asio/async/poll_event.hpp"

namespace lsf {
namespace asio {
namespace async {

template<typename EventDriver>
class BasicProactorSerivce : 
    public basic::NonCopyable,
    public basic::Error
{
public:
    typedef EventDriver     driver_type;

    static const size_t MAX_WAIT_MILLI_SECONDS = 10;

public:
    ////////////////////////////////////////////////////////////
    // Async Funcs
    template<typename SocketType, typename HandlerType>
    bool AsyncAccept(SocketType & socket, HandlerType const & handler)
    {
        if (!socket.SetNonBlock())
        {
            ErrString() = LSF_DEBUG_INFO + socket.ErrString(); 
            return false;
        }

        if (!_driver.RegisterEvent(socket.GetSockFd(), driver_type::FLAG_READ | driver_type::FLAG_ERR))
        {
            ErrString() = LSF_DEBUG_INFO + _driver.ErrString();
            return false;
        }

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

    ////////////////////////////////////////////////////////////
    // Main Routine
    void Run()
    {
        while (true)
        {
            int fd, flag;

            if (!_driver.WaitEvent(MAX_WAIT_MILLI_SECONDS))
            {
                std::cerr << _driver.ErrString() << std::endl;
            }

            while (_driver.GetReadyEvent(&fd, &flag))
            {
                if (flag & driver_type::FLAG_READ)
                {
                    OnReadEvent(fd);
                }
                if (flag & driver_type::FLAG_WRITE)
                {
                    OnWriteEvent(fd);
                }
                if (flag & driver_type::FLAG_ERR)
                {
                    // TODO
                }
            }
        }

    }

    void OnReadEvent(int fd)
    {
        // read data
        //static uint8_t buffer[1024];
        //static size_t  buflen;
        //buflen = sizeof(buffer);


        // call register handler
        CompletionFunc * pfunc = NULL;
        if (!_queue.GetReadCompletionTask(fd, &pfunc)) return;
        pfunc->func();
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
