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

namespace lsf {
namespace asio {
namespace detail {

////////////////////////////////////////////////////////////
// BasicListenSocket
////////////////////////////////////////////////////////////
template<typename TransLayerProtocol>
class BasicListenSocket : public basic::Error
{
public:
    const static int DEF_LISTEN_QUEUE_SIZE = 128;

    typedef BasicSocket<TransLayerProtocol>       socket_type;
    typedef BasicSockAddr<TransLayerProtocol>     sockaddr_type;
    typedef TransLayerProtocol                    proto_type;

public:
    static BasicListenSocket CreateListenSocket(proto_type proto = proto_type::V4())
    {
        int sockfd = ::socket(proto.domain(), proto.type(), proto.protocol());
        return BasicListenSocket(sockfd);
    }

    ////////////////////////////////////////////////////////////
    BasicListenSocket(int sockfd) : _sockfd(sockfd) { }

    BasicListenSocket(BasicListenSocket const & rhs) : _sockfd(rhs._sockfd) { }

    ////////////////////////////////////////////////////////////
    // member funcs
    bool Bind(sockaddr_type const & local) {
        return ErrWrap(::bind(_sockfd, local.Data(), local.DataSize())) == 0;
    }

    bool Listen(int backlog = DEF_LISTEN_QUEUE_SIZE) {
        return ErrWrap(::listen(_sockfd, backlog)) == 0;
    }

    bool Accept(socket_type & socket) {
        int      sockfd;
        if ((sockfd = ErrWrap(::accept(_sockfd, NULL, NULL))) >= 0) {
            socket.SetSockFd(sockfd);
            return true;
        }
        return false;
    }

    bool  Close() {
        return ErrWrap(::close(_sockfd)) == 0;
    }

    sockaddr_type LocalSockAddr() { 
        sockaddr addr;
        socklen_t   addrlen = sizeof(addr);
        ErrWrap(::getsockname(_sockfd, &addr, &addrlen));
        return sockaddr_type(&addr);
    }

    sockaddr_type RemoteSockAddr() {
        sockaddr addr;
        socklen_t   addrlen = sizeof(addr);
        if (ErrWrap(::getpeername(_sockfd, &addr, &addrlen)) == 0)
            return sockaddr_type(&addr);
        else if (IsV4())
            return sockaddr_type(proto_type::V4());
        else
            return sockaddr_type(proto_type::V6());
    }

    ////////////////////////////////////////////////////////////
    // async funcs
    template<typename ServiceType, typename HandlerType>
    bool AsyncAccept(ServiceType & io_service, HandlerType const & handler)
    {
        return io_service.AsyncAccept(*this, handler);
    }

    template<typename ServiceType>
    void CloseAsync(ServiceType & io_service)
    {
        io_service.CloseAsync(_sockfd);
    }

    ////////////////////////////////////////////////////////////
    // SetSockOpt funcs
    bool SetNonBlock() {
        return ErrWrap(::fcntl(_sockfd, F_SETFL, ::fcntl(_sockfd, F_GETFL) | O_NONBLOCK)) == 0;
    }

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

    int  GetSockFd() const { return _sockfd; }

    ////////////////////////////////////////////////////////////
    // bool func
    bool IsV4() { return LocalSockAddr().IsV4(); }
    bool IsV6() { return LocalSockAddr().IsV6(); }

private:
    int             _sockfd;
};

} // end of namespace detail
} // end of namespace asio
} // end of namespace lsf

// vim:ts=4:sw=4:et:ft=cpp:
