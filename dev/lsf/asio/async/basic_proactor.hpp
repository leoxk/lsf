// File:        basic_proactor.hpp
// Description: ---
// Notes:       ---
// Author:      leoxiang <leoxiang@tencent.com>
// Revision:    2012-06-12 by leoxiang

#pragma once

#include "lsf/asio/async/epoll_event.hpp"
#include "lsf/asio/async/poll_event.hpp"

namespace lsf {
namespace asio {
namespace detail {

template<typename EventDriver = EpollEvent<128> >
class BasicProactorSerivce
{
public:
    typedef EventDriver     driver_type;

    bool AsyncAccept();
    bool AsyncConnect();
    bool AsyncRead();
    bool AsyncWrite();
    bool AsyncTimer();

    void Run();

private:
    driver_type     _driver;
};

} // end of namespace detail
} // end of namespace asio
} // end of namespace lsf

// vim:ts=4:sw=4:et:ft=cpp:
