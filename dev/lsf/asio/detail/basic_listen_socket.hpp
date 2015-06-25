// File:        basic_listen_socket.hpp
// Description: ---
// Notes:       ---
// Author:      leoxiang <leoxiang727@qq.com>
// Revision:    2012-06-08 by leoxiang

#pragma once

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "lsf/basic/error.hpp"
#include "lsf/asio/detail/basic_sockaddr.hpp"
#include "lsf/asio/detail/basic_socket.hpp"

namespace lsf {
namespace asio {
namespace detail {

////////////////////////////////////////////////////////////
// BasicListenSocket
////////////////////////////////////////////////////////////
template <typename ProtoType = DummyProtoType>
class BasicListenSocket : public basic::Error {
public:
    const static int DEF_LISTEN_QUEUE_SIZE = 128;

    typedef BasicSocket<ProtoType> socket_type;
    typedef BasicSockAddr<ProtoType> sockaddr_type;
    typedef ProtoType proto_type;

    template <typename OtherProtoType>
    friend class BasicListenSocket;

public:
    static BasicListenSocket CreateListenSocket(proto_type proto = proto_type::V4()) {
        int sockfd = ::socket(proto.domain(), proto.type(), proto.protocol());
        return BasicListenSocket(sockfd);
    }

    ////////////////////////////////////////////////////////////
    BasicListenSocket() {}
    BasicListenSocket(int sockfd) : _sockfd(sockfd) {}

    template <typename OtherProtoType>
    BasicListenSocket(BasicListenSocket<OtherProtoType> const& rhs)
        : _sockfd(rhs._sockfd) {}

    template <typename OtherProtoType>
    BasicListenSocket<ProtoType>& operator=(BasicListenSocket<OtherProtoType> const& rhs) {
        if (this == &rhs) return *this;
        _sockfd = rhs._sockfd;
        return *this;
    }

    ////////////////////////////////////////////////////////////
    bool Bind(sockaddr_type const& local) { return ErrWrap(::bind(_sockfd, local.data(), local.size())) == 0; }

    bool Listen(int backlog = DEF_LISTEN_QUEUE_SIZE) { return ErrWrap(::listen(_sockfd, backlog)) == 0; }

    bool Accept(socket_type& socket) {
        int sockfd;
        if ((sockfd = ErrWrap(::accept4(_sockfd, nullptr, nullptr, SOCK_NONBLOCK))) < 0) return false;
        socket._sockfd = sockfd;
        return true;
    }

    void Close() { if (_sockfd < 0) return; ::close(_sockfd); }

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
    // async funcs
    template <typename ServiceType, typename HandlerType>
    bool AsyncAccept(ServiceType& io_service, HandlerType const& handler) {
        return io_service.AsyncAccept(*this, handler);
    }

    template <typename ServiceType>
    void CloseAsync(ServiceType& io_service) {
        io_service.CloseAsync(_sockfd);
    }

    ////////////////////////////////////////////////////////////
    // SetSockOpt funcs
    bool SetNonBlock() { return ErrWrap(::fcntl(_sockfd, F_SETFL, ::fcntl(_sockfd, F_GETFL) | O_NONBLOCK)) == 0; }

    bool SetSockReuse() {
        size_t optval = 1;
        return ErrWrap(::setsockopt(_sockfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval))) == 0;
    }

    size_t GetRecvBufLen() const {
        size_t optval;
        socklen_t optlen;
        ::getsockopt(_sockfd, SOL_SOCKET, SO_RCVBUF, &optval, &optlen);
        return optval;
    }

    size_t GetSendBufLen() const {
        size_t optval;
        socklen_t optlen;
        ::getsockopt(_sockfd, SOL_SOCKET, SO_SNDBUF, &optval, &optlen);
        return optval;
    }

    int GetSockFd() const { return _sockfd; }

    bool IsV4() { return LocalSockAddr().IsV4(); }
    bool IsV6() { return LocalSockAddr().IsV6(); }

    bool operator!() const { return _sockfd >= 0; }

private:
    int _sockfd = -1;
};

}  // end of namespace detail
}  // end of namespace asio
}  // end of namespace lsf

// vim:ts=4:sw=4:et:ft=cpp:
