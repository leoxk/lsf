// File:        accept_client_msg_service.h
// Description: ---
// Notes:       ---
// Author:      leoxiang <leoxiang727@qq.com>
// Revision:    2015-06-23 by leoxiang

#include <unordered_map>
#include "svr/common/common_proto.h"
#include "svr/common/basic_service.h"

////////////////////////////////////////////////////////////
// AcceptClientMsgService
class AcceptClientMsgService : public BasicAcceptService, public lsf::basic::Singleton<AcceptClientMsgService> {
public:
    struct SockInfo {
        lsf::asio::SharedSocket socket;
        msg::ConnHead conn_head;
        uint64_t last_heart_beat_time = 0;
    };
    using sock_map_type = std::unordered_map<int, SockInfo>;
    static const size_t DEF_HEART_BEAT_INTERVAL = 10*1000;
    static const size_t DEF_HEART_BEAT_TIMEOUT = 15*1000;

    AcceptClientMsgService() : BasicAcceptService(conf::SERVICE_TYPE_CLIENT_MSG) {}
    bool SendMessageToClient(msg::CS& message);

protected:
    virtual bool OnRun();
    virtual void OnConnectionCreate(lsf::asio::SharedSocket socket);
    virtual void OnConnectionMessage(lsf::asio::SharedSocket socket, std::string& message);
    virtual void OnConnectionClose(lsf::asio::SharedSocket socket);
    virtual void OnConnectionPeerClose(lsf::asio::SharedSocket socket);

    void HandleLogin(lsf::asio::SharedSocket socket, msg::CS& message);
    void HandleHeartBeat(lsf::asio::SharedSocket socket, msg::CS& message);
    void HandleTimeSync(lsf::asio::SharedSocket socket, msg::CS& message);
    void HandleOther(lsf::asio::SharedSocket socket, msg::CS& message);

protected:
    sock_map_type _sock_map;
    msg::LoginRsp _login_rsp;
};

// vim:ts=4:sw=4:et:ft=cpp:
