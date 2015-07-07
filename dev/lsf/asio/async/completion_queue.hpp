// File:        completion_queue.hpp
// Description: ---
// Notes:       ---
// Author:      leoxiang <leoxiang727@qq.com>
// Revision:    2012-06-12 by leoxiang

#pragma once

#include <map>
#include <functional>
#include "lsf/basic/noncopyable.hpp"
#include "lsf/basic/error.hpp"
#include "lsf/asio/detail/basic_socket.hpp"
#include "lsf/asio/detail/basic_listen_socket.hpp"

namespace lsf {
namespace asio {
namespace async {

////////////////////////////////////////////////////////////
// Function Closure
////////////////////////////////////////////////////////////
class BasicCompletionFunc {
public:
    static const short ACTION_ACCEPT          = 1;
    static const short ACTION_CONNECT         = 2;
    static const short ACTION_WRITE           = 4;
    static const short ACTION_READ            = 5;
    static const short ACTION_TIMER           = 7;
    using Socket = detail::BasicSocket<>;
    using ListenSocket = detail::BasicListenSocket<>;
    using SockAddr = detail::BasicSockAddr<>;
    using accept_func_type          = std::function<bool(Socket, ListenSocket)>;
    using connect_func_type         = std::function<bool(Socket, SockAddr const&)>;
    using connect_fail_func_type    = std::function<void(Socket, SockAddr const&)>;
    using write_func_type           = std::function<bool(Socket, std::string const&)>;
    using read_func_type            = std::function<bool(Socket, std::string const&)>;
    using read_peer_close_func_type = std::function<void(Socket)>;
    using timer_func_type           = std::function<bool(int)>;
};

class ReadCompletionFunc : public BasicCompletionFunc {
public:
    accept_func_type          accept_func;
    read_func_type            read_func;
    read_peer_close_func_type read_peer_close_func;
    timer_func_type           timer_func;

    short action = 0;
    size_t timer_count = 0;
};

class WriteCompletionFunc : public BasicCompletionFunc {
public:
    connect_func_type         connect_func;
    connect_fail_func_type    connect_fail_func;
    write_func_type           write_func;

    short action = 0;
    SockAddr sockaddr;
    std::string buffer;
};

////////////////////////////////////////////////////////////
// Completion Queue
////////////////////////////////////////////////////////////
class CompletionQueue : public lsf::basic::NonCopyable, public lsf::basic::Error {
public:
    using read_func_map_type  = std::map<int, ReadCompletionFunc>;
    using write_func_map_type = std::map<int, WriteCompletionFunc>;

public:
    ReadCompletionFunc* GetReadCompletionFunc(int fd) {
        auto iter = _read_func_map.find(fd);
        return iter == _read_func_map.end() ? nullptr : &iter->second;
    }

    WriteCompletionFunc* GetWriteCompletionFunc(int fd) {
        auto iter = _write_func_map.find(fd);
        return iter == _write_func_map.end() ? nullptr : &iter->second;
    }

    ReadCompletionFunc&  GetAndCreateReadCompletionFunc(int fd)  { return _read_func_map[fd]; }
    WriteCompletionFunc& GetAndCreateWriteCompletionFunc(int fd) { return _write_func_map[fd]; }

    void CancelCompletionFunc(int fd)  {
        _read_func_map.erase(fd);
        _write_func_map.erase(fd);
    }

private:
    read_func_map_type  _read_func_map;
    write_func_map_type _write_func_map;
};

}  // end of namespace async
}  // end of namespace asio
}  // end of namespace lsf

// vim:ts=4:sw=4:et:ft=cpp:
