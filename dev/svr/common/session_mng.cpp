// File:        session_mng.cpp
// Description: ---
// Notes:       ---
// Author:      leoxiang <leoxiang727@qq.com>
// Revision:    2015-06-15 by leoxiang

#include "svr/common/session_mng.h"
#include "svr/common/common_header.h"

using namespace lsf::util;

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
    tmp = "sess_id=" + TypeCast<std::string>(base_type::sess_id()) + ", sess_type=" +
          TypeCast<std::string>(base_type::sess_type()) + ", sess_state=" +
          TypeCast<std::string>(base_type::sess_state()) + ", player_uid=" +
          TypeCast<std::string>(base_type::player_uid());
    return tmp.c_str();
}

////////////////////////////////////////////////////////////
// Session Manager
bool SessionManager::Init(key_t shm_key, uint32_t max_size) {
    // call base init
    if (!base_type::Init(shm_key, max_size)) return false;

    // recover index
    for (base_type::iterator iter = _pool.begin(); iter != _pool.end(); iter++) {
        _index[iter->sess_id()] = iter.GetPos();
        _cur_max_id = std::max(_cur_max_id, iter->sess_id());
    }

    return true;
}

// Session * SessionManager::CreateSession(uint32_t session_type)
//{
//// check full
// if (_pool.full())
//{
//// TODO
// LSF_LOG_ERR("session is full, size=%d, max_size=%d", _pool.size(), _pool.max_size());
// return false;
//}

//// session_id是否已经存在
// uint32_t session_id = ++_cur_max_id;
//_cur_max_id = _cur_max_id & 0xffffff;
// if (_index.find(session_id) != _index.end())
//{
// LSF_LOG_ERR("create session when session_id already exist, session_id=%u", session_id);
// return false;
//}

//// 创建对象
// uint32_t index = _pool.Malloc();
// Session * psession = _pool.GetPtr(index);

//// 初始化
// psession->set_id(session_id);
// psession->set_timer_id(-1);
// psession->set_type(session_type);
// psession->set_create_time(GetTime());

//// 加入索引
//_index[session_id] = index;

// struct timeval start_time;
// gettimeofday(&start_time, nullptr);
// psession->SetStartTime(start_time);
// return psession;
//}

// void SessionManager::ReleaseSession(uint32_t session_id)
//{
//// 是否有session
// index_type::iterator it = _index.find(session_id);
// if (it == _index.end())
//{
// LSF_LOG_ERR("cant find session, session_id=%u", session_id);
// return;
//}

// Session * psession = _pool.GetPtr(it->second);
// if (psession == nullptr)
//{
// LSF_LOG_ERR("cant find session but index exist, session_id=%d", session_id);
//_index.erase(it);
// return;
//}

////记录session的统计数据
// int32 iTimeSpace;
// struct timeval end_time;
// gettimeofday(&end_time, nullptr);
// struct timeval & start_time = psession->GetStartTime();
// iTimeSpace = (end_time.tv_sec - start_time.tv_sec)*1000 + (end_time.tv_usec - start_time.tv_usec)/1000;

// m_uiDealSessionCount ++;

// for (int i = 0; i < 10; ++i)
//{
// if (m_arrMaxDealTime[i] < iTimeSpace)
//{
// m_arrMaxDealTime[i] = iTimeSpace;
// m_arrMaxDealTimeSession[i].set_type(psession->type());
// m_arrMaxDealTimeSession[i].set_id(psession->id());
// m_arrMaxDealTimeSession[i].set_create_time(psession->create_time());
// break;
//}
//}

//// 释放定时器
// PokerMsgBasic::ClearSessionTimer(psession);

//// 释放对象
//_pool.Free(it->second);

//// 删除索引
//_index.erase(it);

//}

// Session * SessionManager::GetSession(uint32_t session_id)
//{
//// 检查uid是否存在
// index_type::iterator it = _index.find(session_id);
// if (it == _index.end()) return nullptr;

// return _pool.GetPtr(it->second);
//}

// void SessionManager::CheckLeak()
//{
// uint32_t cur_time = GetTime();
// std::vector<uint32_t> del_vec;

//// 找到泄漏的session
// for (pool_type::iterator iter = _pool.Begin(); iter != _pool.End(); ++iter)
//{
// uint32_t diff_time = cur_time - iter->create_time();
// if (diff_time > 10)
//{
// LSF_LOG_ERR("session leak, %s", iter->ToString());
// del_vec.push_back(iter->id());
//}
//}

//// 释放泄漏的session
// for (std::vector<uint32_t>::iterator iter = del_vec.begin(); iter != del_vec.end(); ++iter)
//{
// ReleaseSession(*iter);
//}
//}

// vim:ts=4:sw=4:et:ft=cpp:
