// File:        basic_server.h
// Description: ---
// Notes:       ---
// Author:      leoxiang <leoxiang727@qq.com>
// Revision:    2015-06-08 by leoxiang

#include "lsf/asio/io_service.hpp"
#include "lsf/basic/noncopyable.hpp"

class BasicServer : public lsf::basic::NonCopyable
{
public:
    BasicServer() : _use_epoll(true) { }

    void UseEpoll() { _use_epoll = true; }
    void UsePoll() { _use_epoll = false; }

    void Run(int argc, char** argv);

protected:
    virtual bool Init();

    virtual bool RoutineCheck();

protected:
    bool _use_epoll;
    lsf::asio::EpollService _epoll_service;
    lsf::asio::PollService  _poll_service;
};

// vim:ts=4:sw=4:et:ft=cpp:
