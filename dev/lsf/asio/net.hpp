// File:        net.hpp
// Description: ---
// Notes:       ---
// Author:      leoxiang <leoxiang727@qq.com>
// Revision:    2015-05-19 by leoxiang

#pragma once

#include <unordered_map>
#include "lsf/basic/singleton.hpp"
#include "lsf/asio/async/proactor_service.hpp"
#include "lsf/asio/async/epoll_event_driver.hpp"
#include "lsf/asio/async/poll_event_driver.hpp"

namespace lsf {
namespace asio {

using async::ProactorSerivce;

using Address = detail::BasicAddress;
using SockAddr = detail::BasicSockAddr<>;
using Socket = detail::BasicSocket<>;
using ListenSocket = detail::BasicListenSocket<>;

////////////////////////////////////////////////////////////
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
        IOServiceContent() {}

    public:
        bool use_epoll = false;
        ProactorSerivce *epoll_service = nullptr;
        ProactorSerivce *poll_service = nullptr;
    };

protected:
    IOService() {}                            // construtor is hidden
    IOService(IOService const &);             // copy constructor is hidden
    IOService &operator=(IOService const &);  // copy assignment is hidde
};

}  // end of namespace asio
}  // end of namespace lsf

////////////////////////////////////////////////////////////
// define hash function
namespace std {
template<>
struct hash<lsf::asio::Socket> {
public:
    size_t operator()(lsf::asio::Socket const &sock) const {
        return std::hash<int>()(sock.GetSockFd());
    }
};
} // end namespace std

// vim:ts=4:sw=4:et:
