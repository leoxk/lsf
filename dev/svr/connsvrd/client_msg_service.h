// File:        client_msg_service.h
// Description: ---
// Notes:       ---
// Author:      leoxiang <leoxiang727@qq.com>
// Revision:    2015-06-23 by leoxiang

#include <unordered_map>
#include "lsf/basic/singleton.hpp"
#include "svr/common/basic_service.h"
#include "svr/common/common_proto.h"

////////////////////////////////////////////////////////////
// AcceptClientMsgService
class AcceptClientMsgService : public BasicAcceptService, public lsf::basic::Singleton<AcceptClientMsgService> {
public:
    typedef std::map<lsf::asio::Socket, msg::TcpHead> sock_map_type;

public:
    AcceptClientMsgService() : BasicAcceptService(conf::SERVICE_TYPE_CLIENT_MSG) {}

    bool SendResposeToClient(std::string &message);

protected:
    virtual bool OnConnectionCreate(lsf::asio::Socket socket);
    virtual bool OnConnectionMessage(lsf::asio::Socket socket, std::string& message);
    virtual bool OnConnectionPeerClose(lsf::asio::Socket socket);

protected:
    sock_map_type _sock_map;
};

// vim:ts=4:sw=4:et:ft=cpp:
