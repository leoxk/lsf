// File:        epoll_service.hpp
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

typedef detail::BasicSocket<> Socket;

namespace {

static async::ProactorSerivce epoll_service(new async::EpollEventDriver());
static async::ProactorSerivce poll_service(new async::PollEventDriver());

}

} // end of namespace asio
} // end of namespace lsf

// vim:ts=4:sw=4:et:
