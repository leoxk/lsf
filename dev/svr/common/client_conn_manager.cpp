// File:        client_conn_manager.cpp
// Description: ---
// Notes:       ---
// Author:      leoxiang <leoxiang727@qq.com>
// Revision:    2015-07-21 by leoxiang

#include <set>
#include "svr/common/client_conn_manager.h"
#include "svr/common/common_service.h"

using namespace lsf::basic;
using namespace lsf::asio;

////////////////////////////////////////////////////////////
// ClientConn
bool ClientConn::Serialize(void *buf, size_t buflen, size_t &uselen) const {
    if (!lsf::util::SerializeProtobuf(buf, buflen, uselen, *this)) return false;
    return true;
}

bool ClientConn::UnSerialize(void *buf, size_t buflen, size_t &uselen) {
    if (!lsf::util::UnSerializeProtobuf(buf, buflen, uselen, *this)) return false;
    return true;
}

std::string ClientConn::ToString() const {
    static std::string tmp;
    tmp = "conn_id=" + TypeCast<std::string>(base_type::conn_head().conn_id()) +
          ", player_uid=" + TypeCast<std::string>(base_type::conn_head().uid()) +
          ", conn_address=" + base_type::conn_head().conn_ip() + "|" + TypeCast<std::string>(base_type::conn_head().conn_port());
    return tmp;
}

///////////////////////////////////////////
// Client Connection Manager
bool ClientConnManager::Init(key_t shm_key, uint32_t max_size, bool enable_uid_index) {
    // set option
    _enable_uid_index = enable_uid_index;

    // call base init
    if (!base_type::Init(shm_key, max_size)) return false;

    // recovery uid index if needed
    if (_enable_uid_index) {
        for (auto& pair : *this) {
            _uid_to_conn_id_index.emplace(pair.second.conn_head().uid(), pair.second.conn_head().conn_id());
        }
    }

    return true;
}

ClientConn* ClientConnManager::CreateClientConn(msg::CS const& message) {
    // check full
    if (base_type::full()) {
        LSF_LOG_ERR("connection is full, size=%u, max_size=%u", base_type::size(), base_type::max_size());
        return nullptr;
    }

    // create connection
    ClientConn& client_conn = base_type::operator[](message.conn_head().conn_id());

    // init connection
    client_conn.mutable_conn_head()->CopyFrom(message.conn_head());
    client_conn.mutable_conn_head()->CopyFrom(message.conn_head());

    // update index
    if (_enable_uid_index) {
        _uid_to_conn_id_index.emplace(client_conn.conn_head().uid(), client_conn.conn_head().conn_id());
    }

    // callback
    for (auto& func : _conn_create_func_set) {
        func(client_conn);
    }

    return &client_conn;
}

void ClientConnManager::ReleaseClientConn(uint32_t conn_id) {
    // check existence
    if (!base_type::exist(conn_id)) {
        LSF_LOG_ERR("cant find connection, conn_id=%u", conn_id);
        return;
    }

    // callback
    ClientConn& client_conn = base_type::operator[](conn_id);
    for (auto& func : _conn_peer_close_func_set) {
        func(client_conn);
    }

    // update index
    if (_enable_uid_index) {
        auto pair = _uid_to_conn_id_index.equal_range(client_conn.conn_head().uid());
        for (auto iter = pair.first; iter != pair.second; ++iter) {
            if (iter->second == client_conn.conn_head().conn_id()) {
                _uid_to_conn_id_index.erase(iter);
                break;
            }
        }
    }

    // free object
    base_type::erase(conn_id);
}

ClientConn* ClientConnManager::GetClientConn(uint32_t conn_id) {
    auto iter = base_type::find(conn_id);
    return iter == base_type::end() ? nullptr : &iter->second;
}

ClientConn* ClientConnManager::GetClientConnByUid(uint64_t player_uid) {
    // find single one
    if (!_enable_uid_index) return nullptr;
    auto iter = _uid_to_conn_id_index.find(player_uid);
    if (iter == _uid_to_conn_id_index.end()) return nullptr;
    return GetClientConn(iter->second);
}

// vim:ts=4:sw=4:et:ft=cpp:
