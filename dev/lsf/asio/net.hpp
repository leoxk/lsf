// File:        net.hpp
// Description: ---
// Notes:       ---
// Author:      leoxiang <leoxiang727@qq.com>
// Revision:    2015-05-19 by leoxiang

#pragma once

#include "lsf/basic/singleton.hpp"
#include "lsf/asio/async/proactor_service.hpp"
#include "lsf/asio/async/epoll_event_driver.hpp"
#include "lsf/asio/async/poll_event_driver.hpp"

namespace lsf {
namespace asio {

using async::AsyncInfo;
using async::ProactorSerivce;

typedef detail::BasicAddress Address;
typedef detail::BasicSockAddr<> SockAddr;
typedef detail::BasicSocket<> Socket;
typedef detail::BasicListenSocket<> ListenSocket;

////////////////////////////////////////////////////////////
// IOService
class IOService {
public:
    static ProactorSerivce *Instance() {
        if (IOServiceContent::Instance()->use_epoll) {
            if (IOServiceContent::Instance()->epoll_service == nullptr)
                IOServiceContent::Instance()->epoll_service = new ProactorSerivce(new async::EpollEventDriver);
            return IOServiceContent::Instance()->epoll_service;
        } else {
            if (IOServiceContent::Instance()->poll_service == nullptr)
                IOServiceContent::Instance()->poll_service = new ProactorSerivce(new async::PollEventDriver);
            return IOServiceContent::Instance()->poll_service;
        }
    }

    static ProactorSerivce &Reference() { return *Instance(); }

    static void UseEpoll() { IOServiceContent::Instance()->use_epoll = true; }
    static void UsePoll() { IOServiceContent::Instance()->use_epoll = false; }

protected:
    ////////////////////////////////////////////////////////////
    // IOService Content
    class IOServiceContent : public lsf::basic::Singleton<IOServiceContent> {
    public:
        IOServiceContent() : use_epoll(false), epoll_service(nullptr), poll_service(nullptr) {}

    public:
        bool use_epoll;
        ProactorSerivce *epoll_service;
        ProactorSerivce *poll_service;
    };

protected:
    IOService() {}                            // construtor is hidden
    IOService(IOService const &);             // copy constructor is hidden
    IOService &operator=(IOService const &);  // copy assignment is hidde
};

}  // end of namespace asio
}  // end of namespace lsf

// vim:ts=4:sw=4:et:
