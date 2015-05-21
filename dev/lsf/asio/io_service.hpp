// File:        epoll_service.hpp
// Description: ---
// Notes:       ---
// Author:      leoxiang <leoxiang727@qq.com>
// Revision:    2015-05-19 by leoxiang

#include "lsf/basic/singleton.hpp"
#include "lsf/asio/async/basic_proactor.hpp"
#include "lsf/asio/async/epoll_event.hpp"
#include "lsf/asio/async/poll_event.hpp"

namespace lsf {
namespace asio {

using async::AsyncInfo;

typedef async::BasicProactorSerivce<async::EpollEvent> EpollIOService;

typedef async::BasicProactorSerivce<async::PollEvent>  PollIOService;


} // end of namespace asio
} // end of namespace lsf

// vim:ts=4:sw=4:et:
