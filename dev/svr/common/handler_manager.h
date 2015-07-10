// File:        handler_manager.h
// Description: ---
// Notes:       ---
// Author:      leoxiang <leoxiang727@qq.com>
// Revision:    2015-07-07 by leoxiang

#pragma once
#include "svr/common/common_header.h"
#include "svr/common/basic_handler.h"

////////////////////////////////////////////////////////////
// HandlerManager
class HandlerManager : public lsf::basic::Singleton<HandlerManager> {
public:
    static const size_t DEF_MAX_HANDLER_NUM = 5000;
    using handler_map_type = std::map<int,BasicHandler*>;
    using session_rsp_map_type = std::map<conf::ENServerType,std::pair<data::ENSessionState,data::ENSessionState>>;
    using session_wait_map_type = std::map<data::ENSessionState,data::ENSessionState>;

public:
    HandlerManager();
    bool IsRequest(uint32_t msg_type) { return _handler_map.find(msg_type) != _handler_map.end(); }

    void ProcessSession(Session& session);
    void ProcessSessionResponse(Session& session);
    void ProcessSessionTimeout(Session& session);

    void AddHandler(uint32_t msg_type, BasicHandler* phandler) {
        if (_handler_map.find(msg_type) != _handler_map.end()) {
            LSF_LOG_FATAL("duplicated msg id, %d", msg_type);
        }
        _handler_map[msg_type] = phandler;
    }

private:
    void AddSessionRspMap(conf::ENServerType server_type, data::ENSessionState rsp_state, data::ENSessionState timeout_state) {
        if (_session_rsp_map.find(server_type) != _session_rsp_map.end()) {
            LSF_LOG_FATAL("duplicated server type, %u", server_type);
        }
        _session_rsp_map[server_type] = std::make_pair(rsp_state, timeout_state);
    }

    void AddSessionWaitMap(data::ENSessionState wait_state, data::ENSessionState timeout_state) {
        if (_session_wait_map.find(wait_state) != _session_wait_map.end()) {
            LSF_LOG_FATAL("duplicated session state, %u", wait_state);
        }
        _session_wait_map[wait_state] = timeout_state;
    }

private:
    handler_map_type _handler_map;
    session_rsp_map_type _session_rsp_map;
    session_wait_map_type _session_wait_map;
};


// vim:ts=4:sw=4:et:ft=cpp:
