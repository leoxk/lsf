// File:        session_manager.cpp
// Description: ---
// Notes:       ---
// Author:      leoxiang <leoxiang727@qq.com>
// Revision:    2015-06-15 by leoxiang

#include "svr/common/session_manager.h"
#include "svr/common/common_header.h"

using namespace lsf::util;
using namespace lsf::asio;

////////////////////////////////////////////////////////////
// Session
bool Session::Serialize(void *buf, size_t buflen, size_t &uselen) {
    if (!lsf::util::SerializeProtobuf(buf, buflen, uselen, *this)) return false;
    return true;
}

bool Session::UnSerialize(void *buf, size_t buflen, size_t &uselen) {
    if (!lsf::util::UnSerializeProtobuf(buf, buflen, uselen, *this)) return false;
    return true;
}

std::string Session::ToString() const {
    static std::string tmp;
    tmp = "sess_id=" + TypeCast<std::string>(base_type::session_id()) + ", sess_type=" +
          TypeCast<std::string>(base_type::session_type()) + ", sess_state=" +
          TypeCast<std::string>(base_type::session_state()) + ", player_uid=" +
          TypeCast<std::string>(base_type::player_uid());
    return tmp.c_str();
}

////////////////////////////////////////////////////////////
// Session Manager
bool SessionManager::Init(key_t shm_key, uint32_t max_size) {
    // call base init
    if (!base_type::Init(shm_key, max_size)) return false;

    // recover max id
    _cur_max_id = 0;
    for (auto const & pair : *this) { _cur_max_id = std::max(_cur_max_id, pair.second.session_id()); }

    return true;
}

Session * SessionManager::CreateSession(uint32_t session_type) {
    // check full
    if (base_type::full()) {
        LSF_LOG_ERR("session is full, size=%u, max_size=%u", base_type::size(), base_type::max_size());
        return nullptr;
    }

    // find a valid id
    uint32_t session_id;
    do { session_id = ++_cur_max_id & MAX_SESSION_ID; } while (base_type::exist(session_id));

    // create session
    Session & session = base_type::operator[](session_id);

    // init
    session.set_session_id(session_id);
    session.set_session_type(session_type);
    session.set_create_time(IOService::Instance()->GetClockTimeMilli());

    return &session;
}

void SessionManager::ReleaseSession(uint32_t session_id) {
    // check existence
    if (!base_type::exist(session_id)) {
        LSF_LOG_ERR("cant find session, session_id=%u", session_id);
        return;
    }

    // get session
    Session & session = base_type::operator[](session_id);

    // release timer TODO
    // PokerMsgBasic::ClearSessionTimer(psession);

    // free object
    base_type::erase(session_id);
}

Session * SessionManager::GetSession(uint32_t session_id) {
    auto iter = base_type::find(session_id);
    return iter == base_type::end() ? nullptr : &iter->second;
}

void SessionManager::CheckLeak() {
#ifdef _DEBUG
    // find leak session
    std::vector<uint32_t> del_vec;

    for (auto const & pair : *this) {
        auto const & session = pair.second;
        uint64_t diff_time = IOService::Instance()->GetClockTimeMilli() - session.create_time();
        if (diff_time > DEF_SESSION_LEAK_TIME) {
            LSF_LOG_ERR("detect session leak, %s", session.ToCharStr());
            del_vec.push_back(session.session_id());
        }
    }

    // release leak session
    for (auto session_id : del_vec) { ReleaseSession(session_id); }
#endif
}

// vim:ts=4:sw=4:et:ft=cpp:
