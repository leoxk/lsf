// File:        session_mng.h
// Description: ---
// Notes:       ---
// Author:      leoxiang <leoxiang727@qq.com>
// Revision:    2015-06-15 by leoxiang

#include <string>
#include "svr/common/basic_mng.h"
#include "svr/proto/data_mem.pb.h"

////////////////////////////////////////////////////////////
// Session
class Session : public data::Session
{
public:
    typedef data::Session base_type;

public:
    bool Serialize(void * buf, size_t buflen, size_t & uselen);

    bool UnSerialize(void * buf, size_t buflen, size_t &uselen);

    size_t GetSize() const;

    std::string ToString() const;
};

////////////////////////////////////////////////////////////
// Session Manager
class SessionManager : 
    public BasicManager<Session>,
    public lsf::basic::Singleton<SessionManager>
{
public:
    typedef BasicManager<Session> base_type;
    typedef std::unordered_map<uint32_t,uint32_t> index_type;

public:
    bool Init(key_t shm_key, uint32_t max_size);

    Session * CreateSession(uint32_t session_type);

    Session * GetSession(uint32_t session_id);

    void ReleaseSession(uint32_t session_id);

    void CheckLeak();

private:
	index_type  _index;
	uint32_t    _cur_max_id;;
};

// vim:ts=4:sw=4:et:ft=cpp:
