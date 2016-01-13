// File:        completion_queue.hpp
// Description: ---
// Notes:       ---
// Author:      leoxiang <leoxiang727@qq.com>
// Revision:    2012-06-12 by leoxiang

#pragma once
#include <map>
#include <memory>
#include <functional>
#include "lsf/basic/noncopyable.hpp"
#include "lsf/basic/error.hpp"
#include "lsf/asio/socket.hpp"

namespace lsf {
namespace asio {
namespace async {

////////////////////////////////////////////////////////////
// Completion Queue
////////////////////////////////////////////////////////////
class CompletionQueue : public lsf::basic::NonCopyable, public lsf::basic::Error {
public:
    static const short ACTION_ACCEPT          = 1;
    static const short ACTION_CONNECT         = 2;
    static const short ACTION_WRITE           = 4;
    static const short ACTION_READ            = 5;
    static const short ACTION_TIMER           = 7;
    using accept_func_type          = std::function<void(SharedSocket, SharedSocket)>;
    using accept_fail_func_type     = std::function<void(SharedSocket)>;
    using connect_func_type         = std::function<void(SharedSocket, SockAddr const&)>;
    using connect_fail_func_type    = std::function<void(SharedSocket, SockAddr const&)>;
    using write_func_type           = std::function<void(SharedSocket, std::string const&)>;
    using read_func_type            = std::function<void(SharedSocket, std::string const&)>;
    using read_peer_close_func_type = std::function<void(SharedSocket)>;
    using timer_func_type           = std::function<void(int)>;
    using raw_func_type             = std::function<void(int, int)>;

    struct ReadCompletionFunc {
        accept_func_type          accept_func;
        accept_fail_func_type     accept_fail_func;
        read_func_type            read_func;
        read_peer_close_func_type read_peer_close_func;
        timer_func_type           timer_func;
        SharedSocket              shared_socket;
        short  action      = 0;
        int    timer_fd    = -1;
        size_t timer_count = 0;
    };
    struct WriteCompletionFunc {
        connect_func_type      connect_func;
        connect_fail_func_type connect_fail_func;
        write_func_type        write_func;
        SharedSocket           shared_socket;
        SockAddr               sockaddr;
        std::string buffer;
        short action = 0;
    };
    struct RawCompletionFunc {
        raw_func_type raw_func;
    };

    using shared_read_completion_type = std::shared_ptr<ReadCompletionFunc>;
    using shared_write_completion_type = std::shared_ptr<WriteCompletionFunc>;
    using shared_raw_completion_type = std::shared_ptr<RawCompletionFunc>;

public:
    shared_read_completion_type GetReadCompletionFunc(int fd) {
        auto iter = _read_func_map.find(fd);
        return iter == _read_func_map.end() ? shared_read_completion_type() : iter->second;
    }

    shared_write_completion_type GetWriteCompletionFunc(int fd) {
        auto iter = _write_func_map.find(fd);
        return iter == _write_func_map.end() ? shared_write_completion_type() : iter->second;
    }
    shared_raw_completion_type GetRawCompletionFunc(int fd) {
        auto iter = _raw_func_map.find(fd);
        return iter == _raw_func_map.end() ? shared_raw_completion_type() : iter->second;
    }

    shared_read_completion_type GetAndCreateReadCompletionFunc(int fd)  {
        auto& shared_completion = _read_func_map[fd];
        if (!shared_completion) shared_completion.reset(new shared_read_completion_type::element_type());
        return shared_completion;
    }
    shared_write_completion_type GetAndCreateWriteCompletionFunc(int fd) {
        auto& shared_completion = _write_func_map[fd];
        if (!shared_completion) shared_completion.reset(new shared_write_completion_type::element_type());
        return shared_completion;
    }
    shared_raw_completion_type GetAndCreateRawCompletionFunc(int fd) {
        auto& shared_completion = _raw_func_map[fd];
        if (!shared_completion) shared_completion.reset(new shared_raw_completion_type::element_type());
        return shared_completion;
    }

    void CancelReadCompletionFunc(int fd)  { _read_func_map.erase(fd); }
    void CancelWriteCompletionFunc(int fd) { _write_func_map.erase(fd); }
    void CancelRawCompletionFunc(int fd)   { _raw_func_map.erase(fd); }

    std::map<int,shared_read_completion_type> const& GetReadCompletionMap() const { return _read_func_map; }
    std::map<int,shared_write_completion_type> const& GetWriteCompletionMap() const { return _write_func_map; }
    std::map<int,shared_raw_completion_type> const& GetRawCompletionMap() const { return _raw_func_map; }

private:
    std::map<int,shared_read_completion_type>  _read_func_map;
    std::map<int,shared_write_completion_type> _write_func_map;
    std::map<int,shared_raw_completion_type> _raw_func_map;
};

}  // end of namespace async
}  // end of namespace asio
}  // end of namespace lsf

// vim:ts=4:sw=4:et:ft=cpp:
