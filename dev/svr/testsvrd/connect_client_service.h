// File:        connect_client_service.h
// Description: ---
// Notes:       ---
// Author:      leoxiang <leoxiang727@qq.com>
// Revision:    2016-03-11 by leoxiang

#include "svr/common/common_proto.h"
#include "svr/common/common_proto.h"
#include "svr/common/basic_service.h"

////////////////////////////////////////////////////////////
// ConnectClientService
class ConnectClientService :
    public BasicConnectService,
    public data::TestData {
public:
    using base_type = data::TestData;

public:
    ConnectClientService(uint64_t uid) : BasicConnectService(conf::SERVICE_TYPE_CLIENT_BENCHMARK) {
        base_type::set_uid(uid);
    }

protected:
    virtual bool OnRun();
    virtual void OnConnectionCreate(lsf::asio::SharedSocket socket);
    virtual void OnConnectionMessage(lsf::asio::SharedSocket socket, std::string& message);
    virtual void OnConnectionPeerClose(lsf::asio::SharedSocket socket);

    void HandleOther(lsf::asio::SharedSocket socket, msg::CS& message);

protected:
};


// vim:ts=4:sw=4:et:ft=cpp:
