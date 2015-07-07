// File:        session_manager.h
// Description: ---
// Notes:       ---
// Author:      leoxiang <leoxiang727@qq.com>
// Revision:    2015-06-15 by leoxiang

#pragma once
#include <string>
#include "svr/common/basic_manager.h"
#include "svr/proto/data_mem.pb.h"

////////////////////////////////////////////////////////////
// Session
class Session : public data::Session {
public:
    using base_type = data::Session;
    using key_type = decltype(base_type().session_id());

public:
    bool Serialize(void *buf, size_t buflen, size_t &uselen) const;
    bool UnSerialize(void *buf, size_t buflen, size_t &uselen);
    size_t GetSize() const { return base_type::ByteSize(); }
    key_type GetKey() const { return base_type::session_id(); }
    std::string ToString() const;
};

////////////////////////////////////////////////////////////
// SessionManager
class SessionManager : public BasicManager<Session>, public lsf::basic::Singleton<SessionManager> {
public:
    using base_type = BasicManager<Session>;
    static const size_t MAX_SESSION_ID = 0xffffff;
    static const size_t DEF_SESSION_LEAK_TIME = 30*1000; // milli time
    static const size_t DEF_CHECK_SESSION_LEAK_TIME = 30*1000; // milli time

public:
    bool Init(key_t shm_key, uint32_t max_size);
    Session *CreateSession(uint32_t session_type);
    Session *GetSession(uint32_t session_id);
    void ReleaseSession(uint32_t session_id);
    void CheckLeak();

private:
    uint32_t _cur_max_id;
};

// vim:ts=4:sw=4:et:ft=cpp:
