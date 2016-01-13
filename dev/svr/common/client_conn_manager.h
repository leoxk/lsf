// File:        client_conn_manager.h
// Description: ---
// Notes:       ---
// Author:      leoxiang <leoxiang727@qq.com>
// Revision:    2015-07-21 by leoxiang

#pragma once
#include "svr/common/basic_manager.h"
#include "svr/common/common_header.h"
#include "svr/common/common_proto.h"

////////////////////////////////////////////////////////////
// ClientConn
class ClientConn : public data::ClientConn
{
public:
    using base_type = data::ClientConn;
    using key_type = decltype(base_type().conn_head().conn_id());

public:
    bool Serialize(void * buf, size_t buflen, size_t & uselen) const;
    bool UnSerialize(void * buf, size_t buflen, size_t &uselen);
    void AfterUnSerialize() { }
    size_t GetSize() const { return base_type::ByteSize(); }
    key_type GetKey() const { return base_type::conn_head().conn_id(); }
    std::string ToString() const;
};

////////////////////////////////////////////////////////////
class ClientConnManager : public BasicManager<ClientConn>, public lsf::basic::Singleton<ClientConnManager> {
public:
    using base_type = BasicManager<ClientConn>;
    using conn_create_func_type = std::function<void(ClientConn&)>;
    using conn_peer_close_func_type = std::function<void(ClientConn&)>;
    static const size_t DEF_HEART_BEAT_CHECK_INTERVAL = 30 * 1000;

public:
    bool Init(key_t shm_key, uint32_t max_size, bool enable_uid_index = false);
    ClientConn* CreateClientConn(msg::CS const& message);
    ClientConn* GetClientConn(uint32_t conn_id);
    ClientConn* GetClientConnByUid(uint64_t player_uid);
    void ReleaseClientConn(uint32_t conn_id);

public:
    template<typename HandlerType>
    void AddClientConnCreateHandle(HandlerType&& handler) {
        _conn_create_func_set.push_back(std::forward<HandlerType>(handler));
    }

    template<typename HandlerType>
    void AddClientConnPeerCloseHandle(HandlerType&& handler) {
        _conn_peer_close_func_set.push_back(std::forward<HandlerType>(handler));
    }

protected:
    std::vector<conn_create_func_type> _conn_create_func_set;
    std::vector<conn_peer_close_func_type> _conn_peer_close_func_set;
    bool _enable_uid_index = false;
    std::multimap<uint64_t,uint32_t> _uid_to_conn_id_index;
};

// vim:ts=4:sw=4:et:ft=cpp:
