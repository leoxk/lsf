// File:        socket.hpp
// Description: ---
// Notes:
// Author:      leoxiang <leoxiang727@qq.com>
// Revision:    2012-06-07 by leoxiang

#pragma once
#include <fcntl.h>
#include <unistd.h>
#include <sys/select.h>
#include <sys/time.h>
#include <memory>
#include <poll.h>
#include "lsf/basic/error.hpp"
#include "lsf/basic/macro.hpp"
#include "lsf/asio/sockaddr.hpp"
#include "lsf/util/time.hpp"

namespace lsf {
namespace asio {

namespace async {
class ProactorSerivce;
} // end namespace async

////////////////////////////////////////////////////////////
// Socket
////////////////////////////////////////////////////////////
class Socket : public lsf::basic::Error {
public:
    static const int DEF_LISTEN_QUEUE_SIZE = 128;
    static const size_t MAX_BUFFER_LEN = 128 * 1024;
    static const size_t DEF_TIMEOUT = 1000;
    friend lsf::asio::async::ProactorSerivce;

public:
    ////////////////////////////////////////////////////////////
    // constructor
    Socket(proto::Protocol proto, int sockfd, bool is_connect) :
        _proto(proto),
        _sockfd(sockfd),
        _is_connect(is_connect) {}

    Socket(proto::Protocol proto) :
        _proto(proto),
        _sockfd(ErrWrap(::socket(proto.domain().domain(), proto.type(), proto.protocol()))),
        _is_connect(false) {}

    ~Socket() { if (*this) ::close(_sockfd); }

    ////////////////////////////////////////////////////////////
    // common function
    int SockFd() const { return _sockfd; }
    proto::Protocol protocol() const { return _proto; }

    explicit operator bool() const { return _sockfd >= 0; }
    bool IsV4() const { return _proto.IsV4(); }
    bool IsV6() const { return _proto.IsV6(); }
    bool IsConnect() const { return _is_connect; }

    bool operator==(Socket const& rhs) const { return _sockfd == rhs._sockfd; }
    bool operator!=(Socket const& rhs) const { return _sockfd != rhs._sockfd; }
    bool operator< (Socket const& rhs) const { return _sockfd <  rhs._sockfd; }

    ////////////////////////////////////////////////////////////
    // socket function
    bool ShutDown() {
        _is_connect = false;
        return ErrWrap(::shutdown(_sockfd, SHUT_RDWR)) >= 0;
    }

    inline bool Bind(Address const& address, uint16_t port) {
        return Bind(SockAddr(address, port));
    }

    bool Bind(SockAddr const& sockaddr) {
        if (::bind(_sockfd, sockaddr.data(), sockaddr.size()) < 0) {
            SetErrString(SysErrString() + " " + sockaddr.ToString());
            return false;
        }
        return true;
    }

    bool Listen(int backlog = DEF_LISTEN_QUEUE_SIZE) {
        if (::listen(_sockfd, backlog) < 0) {
            SetErrString(SysErrString() + " " + LocalSockAddr().ToString());
            return false;
        }
        return true;
    }

    Socket* Accept() {
        // call accept
        int sockfd;
        if ((sockfd = ErrWrap(::accept4(_sockfd, nullptr, nullptr, SOCK_NONBLOCK))) < 0) return nullptr;

        // caller must be responsible for releasing socket
        return new Socket(_proto, sockfd, true);
    }

    bool CheckSockError() const {
        // check optval from sockopt
        int optval = 0;
        socklen_t optlen = sizeof(optval);
        if (::getsockopt(_sockfd, SOL_SOCKET, SO_ERROR, &optval, &optlen) != 0) optval = errno;
        if (optval != 0) {
            SetErrString(std::strerror(optval));
            return false;
        }
        return true;
    }

    ////////////////////////////////////////////////////////////
    // sync connect
    inline bool Connect(Address const& address, uint16_t port, uint64_t milli_expire = DEF_TIMEOUT) {
        return Connect(SockAddr(address, port), milli_expire);
    }

    bool Connect(SockAddr const& remote, uint64_t milli_expire = DEF_TIMEOUT) {
        // try connect
        int ret = ErrWrap(::connect(_sockfd, remote.data(), remote.size()));

        // success
        if (ret == 0) return true;

        // not because non-block return fail
        if (errno != EINPROGRESS) return false;

        // wait write event
        if (!_WaitActionTimeout(_sockfd, false, true, milli_expire)) return false;

        // check result
        if (!CheckSockError()) return false;

        _is_connect = true;
        return true;
    }

    ////////////////////////////////////////////////////////////
    // sync recv
    bool Recv(std::string& content, uint64_t milli_expire = DEF_TIMEOUT) {
        // check connection
        if (!IsConnect()) {
            SetErrString(std::string(LSF_DEBUG_INFO) + " Socket not connected");
            return false;
        }

        // wait read event
        if (!_WaitActionTimeout(_sockfd, true, false, milli_expire)) return false;

        // read all data
        content.clear();
        while (true) {
            // recv
            char buffer[MAX_BUFFER_LEN];
            int ret = ErrWrap(::recv(_sockfd, buffer, sizeof(buffer), 0));

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
            if (ret == 0) {
                ShutDown();
                break;
            }

            // handle recv data
            content.append(buffer, ret);
        }

        return !content.empty();
    }

    ////////////////////////////////////////////////////////////
    // sync send
    bool Send(std::string const& content, uint64_t milli_expire = DEF_TIMEOUT) {
        return Send(content.data(), content.size(), milli_expire);
    }

    bool Send(void const *buf, size_t len, uint64_t milli_expire = DEF_TIMEOUT) {
        // check connection
        if (!IsConnect()) {
            SetErrString(std::string(LSF_DEBUG_INFO) + " Socket not connected");
            return false;
        }

        // record start time
        uint64_t st_time = lsf::util::Time::Now();

        size_t total = 0;
        while (total < len) {
            // clac remain wait time
            uint64_t milli_diff = lsf::util::Time::Now() - st_time;

            // wait timeout
            if (milli_diff >= milli_expire) return false;

            // wait write event
            if (!_WaitActionTimeout(_sockfd, false, true, milli_expire - milli_diff)) return false;

            int ret = ErrWrap(::send(_sockfd, (char const *)buf + total, len - total, 0));

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

    SockAddr LocalSockAddr() {
        sockaddr addr;
        socklen_t addrlen = sizeof(addr);
        return ErrWrap(::getsockname(_sockfd, &addr, &addrlen)) < 0 ? SockAddr(_proto.domain()) : SockAddr(&addr);
    }

    SockAddr RemoteSockAddr() {
        sockaddr addr;
        socklen_t addrlen = sizeof(addr);
        return ErrWrap(::getpeername(_sockfd, &addr, &addrlen)) < 0 ? SockAddr(_proto.domain()) : SockAddr(&addr);
    }

    ////////////////////////////////////////////////////////////
    // set socket option function
    bool SetNonBlock() {
        return ErrWrap(::fcntl(_sockfd, F_SETFL, ::fcntl(_sockfd, F_GETFL) | O_NONBLOCK)) == 0;
    }

    bool SetBlock() {
        return ErrWrap(::fcntl(_sockfd, F_SETFL, ::fcntl(_sockfd, F_GETFL) & (~O_NONBLOCK))) == 0;
    }

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

    bool SetSendTimeout(timeval const& tv) {
        return ErrWrap(::setsockopt(_sockfd, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(tv))) == 0;
    }

    bool SetRecvTimeout(timeval const& tv) {
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

private:
    bool _WaitActionTimeout(int fd, bool wait_read, bool wait_write, uint64_t milli_expire) {
        // add fd set
        pollfd fds[1];
        fds[0] = {0};
        fds[0].fd = fd;
        fds[0].events |= POLLERR;
        if (wait_read)  fds[0].events |= POLLIN;
        if (wait_write) fds[0].events |= POLLOUT;

        // wait
        while (true) {
            auto ret = ErrWrap(::poll(fds, 1, milli_expire));
            // interrupt, do again
            if (ret < 0 && errno == EINTR) continue;
            // poll error
            if (ret < 0) return false;
            break;
        }

        if (wait_read  && (fds[0].revents & POLLIN))  return true;
        if (wait_write && (fds[0].revents & POLLOUT)) return true;
        return false;
    }
    // bool _WaitActionTimeout(int fd, bool wait_read, bool wait_write, uint64_t milli_expire) {
    //     struct timeval tv;
    //     tv.tv_sec = milli_expire / 1000;
    //     tv.tv_usec = (milli_expire % 1000) * 1000;
    //     fd_set rd_set, wt_set;
    //     FD_ZERO(&rd_set);
    //     FD_ZERO(&wt_set);
    //     FD_SET(fd, &rd_set);
    //     FD_SET(fd, &wt_set);
    //
    //     // process select, avoid signal
    //     int ret;
    //     while (true) {
    //         ret = ErrWrap(::select(fd+1, wait_read ? &rd_set : nullptr, wait_write ? &wt_set : nullptr, nullptr, &tv));
    //         if (ret < 0 && errno == EINTR) continue;
    //         break;
    //     }
    //
    //     // select failed
    //     if (ret < 0) return false;
    //
    //     // no wait action
    //     if (ret == 0) return false;
    //
    //     return true;
    // }

private:
    proto::Protocol _proto;
    int _sockfd;
    bool _is_connect;
};

////////////////////////////////////////////////////////////
// SharedSocket
class SharedSocket : public std::shared_ptr<Socket> {
public:
    using base_type = std::shared_ptr<Socket>;
    using base_type::reset;
    SharedSocket() {}
    SharedSocket(Socket* psocket) : base_type(psocket) {}
    SharedSocket(proto::Protocol proto) : base_type(new Socket(proto)) {}
    void reset(proto::Protocol proto) { base_type::reset(new Socket(proto)); }
};

}  // end of namespace asio
}  // end of namespace lsf

////////////////////////////////////////////////////////////
// define hash function
namespace std {
template<>
struct hash<lsf::asio::Socket> {
    size_t operator()(lsf::asio::Socket const& sock) const {
        return std::hash<int>()(sock.SockFd());
    }
};
} // end namespace std

// vim:ts=4:sw=4:et:ft=cpp:
