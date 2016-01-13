// File:        session_manager.h
// Description: ---
// Notes:       ---
// Author:      leoxiang <leoxiang727@qq.com>
// Revision:    2015-06-15 by leoxiang

#pragma once
#include <string>
#include "svr/common/common_proto.h"
#include "svr/common/basic_manager.h"
#include "svr/common/timer_manager.h"

////////////////////////////////////////////////////////////
// Session
class Session : public data::Session {
public:
    using base_type = data::Session;
    using key_type = decltype(base_type().session_id());

public:
    bool Serialize(void *buf, size_t buflen, size_t &uselen) const;
    bool UnSerialize(void *buf, size_t buflen, size_t &uselen);
    void AfterUnSerialize() { }
    size_t GetSize() const { return base_type::ByteSize(); }
    key_type GetKey() const { return base_type::session_id(); }
    std::string ToString() const;
    char const* GetType() const;

public:
    data::DBHandle&  GetDBHandle()  { return *base_type::mutable_db_handles(base_type::db_process_index()); }
    data::RobotData& GetRobotData() { return *GetDBHandle().mutable_data()->mutable_robot_data();           }
    bool  IsIndexValid(size_t index) { return index < (size_t)base_type::db_handles_size(); }
};

////////////////////////////////////////////////////////////
// SessionManager
class SessionManager : public BasicManager<Session>, public lsf::basic::Singleton<SessionManager> {
public:
    using base_type = BasicManager<Session>;
    static const size_t MAX_SESSION_ID = 0xffffff;
    static const size_t DEF_SESSION_LEAK_TIME = 300*1000; // milli time
    static const size_t DEF_SESSION_LEAK_CHECK_INTERVAL = 30*1000; // milli time

public:
    bool Init(key_t shm_key, uint32_t max_size);
    Session* CreateSession();
    Session* GetSession(uint32_t session_id);
    void ReleaseSession(uint32_t session_id);

private:
    uint32_t _cur_max_id;
};

// vim:ts=4:sw=4:et:ft=cpp:
