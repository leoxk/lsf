// File:        basic_socket.hpp
// Description: ---
// Notes:       
// Author:      leoxiang <leoxiang727@qq.com>
// Revision:    2012-06-07 by leoxiang

#pragma once

#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "lsf/basic/error.hpp"
#include "lsf/asio/detail/basic_sockaddr.hpp"

namespace lsf {
namespace asio {
namespace detail {

////////////////////////////////////////////////////////////
// DummyProtocol
////////////////////////////////////////////////////////////
class DummyProtocol
{
public:
    typedef void net_layer_proto;
    static DummyProtocol V4() { return DummyProtocol(); }
    static DummyProtocol V6() { return DummyProtocol(); }
    int domain()   const { return 0; }
    int type()     const { return 0; }
    int protocol() const { return 0; }
};

////////////////////////////////////////////////////////////
// BasicSocket
////////////////////////////////////////////////////////////
template<typename TransLayerProtocol = DummyProtocol>
class BasicSocket : public basic::Error
{
public:
    typedef BasicSockAddr<TransLayerProtocol>  sockaddr_type;
    typedef TransLayerProtocol                 proto_type;

public:
    ////////////////////////////////////////////////////////////
    BasicSocket(proto_type proto = proto_type::V4()) {
        _sockfd = ErrWrap(::socket(proto.domain(), proto.type(), proto.protocol()));
    }

    BasicSocket(sockaddr_type const & local) {
        if (local.IsV4()) *this = BasicSocket(proto_type::V4());
        else              *this = BasicSocket(proto_type::V6());

        Bind(local);
    }

    BasicSocket(int sockfd) : _sockfd(sockfd) { }

    BasicSocket(BasicSocket const & rhs) : _sockfd(rhs._sockfd) { }

    ////////////////////////////////////////////////////////////
    // member funcs
    bool Bind(sockaddr_type const & local) {
        return ErrWrap(::bind(_sockfd, local.Data(), local.DataSize())) == 0;
    }

    bool Connect(sockaddr_type const & remote) {
        return ErrWrap(::connect(_sockfd, remote.Data(), remote.DataSize())) == 0;
    }

    bool  Close() {
        return ErrWrap(::close(_sockfd)) == 0;
    }

    ssize_t Send(void const * buf, size_t len) {
        return ErrWrap(::send(_sockfd, buf, len, MSG_NOSIGNAL));
    }

    ssize_t SendTo(void const * buf, size_t len, sockaddr_type const & remote) {
        return ErrWrap(::sendto(_sockfd, buf, len, MSG_NOSIGNAL, remote.Data(), remote.DataSize()));
    }

    ssize_t Recv(void * buf, size_t len) {
        return ErrWrap(::recv(_sockfd, buf, len, 0));
    }

    ssize_t RecvFrom(void * buf, size_t len, sockaddr_type & remote) {
        size_t socklen;
        return ErrWrap(::recvfrom(_sockfd, buf, len, 0, remote.Data(), &socklen));
    }

    ////////////////////////////////////////////////////////////
    // async funcs
    template<typename ServiceType, typename SockAddrType, typename HandlerType>
    bool AsyncConnect(ServiceType & io_service, SockAddrType const & sockaddr, HandlerType const & handler)
    {
        return io_service.AsyncConnect(*this, sockaddr, handler);
    }

    template<typename ServiceType, typename HandlerType>
    bool AsyncWrite(ServiceType & io_service, void const * buffer, size_t buflen, HandlerType const & handler)
    {
        return io_service.AsyncWrite(*this, buffer, buflen, handler);
    }

    template<typename ServiceType, typename HandlerType1, typename HandlerType2>
    bool AsyncRead(ServiceType & io_service, HandlerType1 const & read_handler, HandlerType2 const & rdhup_handler)
    {
        return io_service.AsyncRead(*this, read_handler, rdhup_handler);
    }

    template<typename ServiceType, typename HandlerType>
    bool AsyncRead(ServiceType & io_service, HandlerType const & handler)
    {
        return io_service.AsyncRead(*this, handler);
    }

    template<typename ServiceType>
    void CloseAsync(ServiceType & io_service)
    {
        io_service.CloseAsync(_sockfd);
    }

    void AsyncConnect();

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
    // SetSockOpt funcs
    bool SetNonBlock() {
        return ErrWrap(::fcntl(_sockfd, F_SETFL, ::fcntl(_sockfd, F_GETFL) | O_NONBLOCK)) == 0;
    }

    bool SetSendBuf(size_t buflen) {
        return ErrWrap(::setsockopt(_sockfd, SOL_SOCKET, SO_SNDBUF, &buflen, sizeof(buflen))) == 0;
    }

    bool SetRecvBuf(size_t buflen) {
        return ErrWrap(::setsockopt(_sockfd, SOL_SOCKET, SO_RCVBUF, &buflen, sizeof(buflen))) == 0;
    }

    bool SetSockReuse() {
        size_t optval = 1;
        return ErrWrap(::setsockopt(_sockfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval))) == 0;
    }

    bool SetSendTimeout(timeval const & tv) {
        return ErrWrap(::setsockopt(_sockfd, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(tv))) == 0;
    }

    bool SetRecvTimeout(timeval const & tv) {
        return ErrWrap(::setsockopt(_sockfd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv))) == 0;
    }

    bool SetLinger(bool is_on, int sec) {
        struct linger linger;
        linger.l_onoff = is_on;
        linger.l_linger = sec;
        return ErrWrap(::setsockopt(_sockfd, SOL_SOCKET, SO_LINGER, (char const *)&linger, sizeof(linger))) == 0;
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

    void SetSockFd(int sockfd) { _sockfd = sockfd; }

    bool IsV4() { return LocalSockAddr().IsV4(); }
    bool IsV6() { return LocalSockAddr().IsV6(); }

private:
    int             _sockfd;
};

} // end of namespace detail
} // end of namespace asio
} // end of namespace lsf

// vim:ts=4:sw=4:et:ft=cpp:
