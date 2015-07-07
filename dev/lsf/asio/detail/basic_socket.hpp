// File:        basic_socket.hpp
// Description: ---
// Notes:
// Author:      leoxiang <leoxiang727@qq.com>
// Revision:    2012-06-07 by leoxiang

#pragma once

#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/select.h>
#include <sys/time.h>
#include "lsf/basic/error.hpp"
#include "lsf/asio/detail/basic_sockaddr.hpp"
#include "lsf/util/time.hpp"

namespace lsf {
namespace asio {
namespace detail {

// forward declare
template <typename ProtoType>
class BasicListenSocket;

////////////////////////////////////////////////////////////
// BasicSocket
////////////////////////////////////////////////////////////
template <typename ProtoType = DummyProtoType>
class BasicSocket : public lsf::basic::Error {
public:
    using sockaddr_type = BasicSockAddr<ProtoType>;
    using proto_type = ProtoType;

    static const size_t MAX_BUFFER_LEN = 128 * 1024;
    static const size_t DEF_TIMEOUT = 1000;

    template <typename OtherProtoType>
    friend class BasicListenSocket;
    template <typename OtherProtoType>
    friend class BasicSocket;

public:
    static BasicSocket CreateSocket(proto_type proto = proto_type::V4()) {
        int sockfd = ::socket(proto.domain(), proto.type(), proto.protocol());
        return BasicSocket(sockfd);
    }

    ////////////////////////////////////////////////////////////
    // constructor
    BasicSocket() {}
    BasicSocket(int sockfd) : _sockfd(sockfd) {}

    template <typename OtherProtoType>
    BasicSocket(BasicSocket<OtherProtoType> const &rhs)
        : _sockfd(rhs._sockfd) {}

    template <typename OtherProtoType>
    BasicSocket<ProtoType> &operator=(BasicSocket<OtherProtoType> const &rhs) {
        if (this == (decltype(this))&rhs) return *this;
        _sockfd = rhs._sockfd;
        return *this;
    }

    ////////////////////////////////////////////////////////////
    // common func
    bool Bind(sockaddr_type const &local) { return ErrWrap(::bind(_sockfd, local.data(), local.size())) == 0; }

    void Close() { if (_sockfd < 0) return; ::close(_sockfd); }

    ////////////////////////////////////////////////////////////
    // sync connect
    bool Connect(sockaddr_type const &remote, uint64_t milli_expire = DEF_TIMEOUT) {
        int ret = ErrWrap(::connect(_sockfd, remote.data(), remote.size()));

        // success
        if (ret == 0) return true;

        // not because non-block return fail
        if (errno != EINPROGRESS) {
            return false;
        }

        // wait write event
        if (!_WaitActionTimeout(_sockfd, false, true, milli_expire)) return false;

        // check result
        ret = GetSockError();
        if (ret != 0) {
            SetErrorNo(ret);
            return false;
        }

        return true;
    }

    ////////////////////////////////////////////////////////////
    // sync recv
    bool Recv(std::string &content, uint64_t milli_expire = DEF_TIMEOUT) {
        content.clear();

        // wait read event
        if (!_WaitActionTimeout(_sockfd, true, false, milli_expire)) return false;

        while (true) {
            // recv
            static char buffer[MAX_BUFFER_LEN];
            int ret = ::recv(_sockfd, buffer, sizeof(buffer), 0);

            // handle error
            if (ret < 0) {
                if (errno == EINTR) {
                    continue;
                } else if (errno == EAGAIN || errno == EWOULDBLOCK) {
                    break;
                } else {
                    break;
                }
            }

            // handle socket close
            if (ret == 0) break;

            // handle recv data
            content.append(buffer, ret);
        }

        return !content.empty();
    }

    ////////////////////////////////////////////////////////////
    // sync send
    bool Send(std::string const &content, uint64_t milli_expire = DEF_TIMEOUT) {
        return Send(content.data(), content.size(), milli_expire);
    }

    bool Send(void const *buf, size_t len, uint64_t milli_expire = DEF_TIMEOUT) {

        // record start time
        struct timeval start_tv;
        ::gettimeofday(&start_tv, NULL);

        size_t total = 0;
        while (total < len) {
            // clac remain wait time
            struct timeval tv;
            ::gettimeofday(&tv, NULL);
            uint64_t milli_diff = lsf::basic::Time::TimeValDiff(tv, start_tv);

            // wait timeout
            if (milli_diff >= milli_expire) return false;

            // wait write event
            if (!_WaitActionTimeout(_sockfd, false, true, milli_expire > milli_diff)) return false;

            int ret = ::send(_sockfd, (char const *)buf + total, len - total, 0);

            // handle error
            if (ret < 0) {
                if (errno == EINTR) {
                    continue;
                } else {
                    break;
                }
            }

            // handle send data
            total += ret;
        }

        return total == len;
    }

    sockaddr_type LocalSockAddr() {
        sockaddr addr;
        socklen_t addrlen = sizeof(addr);
        ErrWrap(::getsockname(_sockfd, &addr, &addrlen));
        return sockaddr_type(&addr);
    }

    sockaddr_type RemoteSockAddr() {
        sockaddr addr;
        socklen_t addrlen = sizeof(addr);
        if (ErrWrap(::getpeername(_sockfd, &addr, &addrlen)) == 0)
            return sockaddr_type(&addr);
        else if (IsV4())
            return sockaddr_type(proto_type::V4());
        else
            return sockaddr_type(proto_type::V6());
    }

    ////////////////////////////////////////////////////////////
    // SetSockOpt funcs
    bool SetNonBlock() { return ErrWrap(::fcntl(_sockfd, F_SETFL, ::fcntl(_sockfd, F_GETFL) | O_NONBLOCK)) == 0; }

    bool SetBlock() { return ErrWrap(::fcntl(_sockfd, F_SETFL, ::fcntl(_sockfd, F_GETFL) & (~O_NONBLOCK))) == 0; }

    bool SetSendBuf(size_t buflen) {
        int optval = buflen;
        return ErrWrap(::setsockopt(_sockfd, SOL_SOCKET, SO_SNDBUF, &optval, sizeof(optval))) == 0;
    }

    bool SetRecvBuf(size_t buflen) {
        int optval = buflen;
        return ErrWrap(::setsockopt(_sockfd, SOL_SOCKET, SO_RCVBUF, &optval, sizeof(optval))) == 0;
    }

    bool SetSockReuse() {
        int optval = 1;
        return ErrWrap(::setsockopt(_sockfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval))) == 0;
    }

    bool SetSendTimeout(timeval const &tv) {
        return ErrWrap(::setsockopt(_sockfd, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(tv))) == 0;
    }

    bool SetRecvTimeout(timeval const &tv) {
        return ErrWrap(::setsockopt(_sockfd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv))) == 0;
    }

    bool SetLinger(bool is_on, int sec) {
        struct linger linger;
        linger.l_onoff = is_on;
        linger.l_linger = sec;
        return ErrWrap(::setsockopt(_sockfd, SOL_SOCKET, SO_LINGER, (char const *)&linger, sizeof(linger))) == 0;
    }

    size_t GetRecvBufLen() const {
        int optval = 0;
        socklen_t optlen = sizeof(optval);
        ::getsockopt(_sockfd, SOL_SOCKET, SO_RCVBUF, &optval, &optlen);
        return (size_t)optval;
    }

    size_t GetSendBufLen() const {
        int optval = 0;
        socklen_t optlen = sizeof(optval);
        ::getsockopt(_sockfd, SOL_SOCKET, SO_SNDBUF, &optval, &optlen);
        return (size_t)optval;
    }

    int GetSockError() const {
        if (_sockfd < 0) return EBADF;
        int optval = 0;
        socklen_t optlen = sizeof(optval);
        if (::getsockopt(_sockfd, SOL_SOCKET, SO_ERROR, &optval, &optlen) != 0) optval = errno;
        return optval;
    }

    void SetSockFd(int sockfd) { _sockfd = sockfd; }
    int GetSockFd() const { return _sockfd; }

    void Clear() { _sockfd = -1; }

    bool IsV4() { return LocalSockAddr().IsV4(); }
    bool IsV6() { return LocalSockAddr().IsV6(); }

    explicit operator bool() const { return _sockfd >= 0; }

    template <typename OtherProtoType>
    bool operator==(BasicSocket<OtherProtoType> const &rhs) const {
        return _sockfd == rhs._sockfd;
    }

    template <typename OtherProtoType>
    bool operator!=(BasicSocket<OtherProtoType> const &rhs) const {
        return _sockfd != rhs._sockfd;
    }

    template <typename OtherProtoType>
    bool operator<(BasicSocket<OtherProtoType> const &rhs) const {
        return _sockfd < rhs._sockfd;
    }

private:
    bool _WaitActionTimeout(int fd, bool wait_read, bool wait_write, uint64_t milli_expire)
    {
        struct timeval tv;
        tv.tv_sec = milli_expire / 1000;
        tv.tv_usec = milli_expire * 1000;
        fd_set rd_set, wt_set;
        FD_ZERO(&rd_set);
        FD_ZERO(&wt_set);
        FD_SET(fd, &rd_set);
        FD_SET(fd, &wt_set);

        // process select, avoid signal
        int ret;
        while (true)
        {
            ret = ErrWrap(::select(fd+1, wait_read ? &rd_set : nullptr, wait_write ? &wt_set : nullptr, nullptr, &tv));
            if (ret < 0 && errno == EINTR) continue;
            break;
        }

        // select failed
        if (ret < 0) return false;

        // no wait action
        if (ret == 0) return false;

        return true;
    }

private:
    int     _sockfd = -1;
};

}  // end of namespace detail
}  // end of namespace asio
}  // end of namespace lsf

// vim:ts=4:sw=4:et:ft=cpp:
