// File:        handler_manager.h
// Description: ---
// Notes:       ---
// Author:      leoxiang <leoxiang727@qq.com>
// Revision:    2015-07-07 by leoxiang

#pragma once
#include <array>
#include "svr/common/common_header.h"
#include "svr/common/basic_handler.h"

////////////////////////////////////////////////////////////
// DefaultHandler
class DefaultHandler : public BasicHandler {
public:
    virtual data::ENSessionState OnClientRequest(Session& session);
    virtual data::ENSessionState OnServerRequest(Session& session);
};

////////////////////////////////////////////////////////////
// HandlerManager
class HandlerManager : private lsf::basic::Singleton<HandlerManager> {
public:
    using base_type = lsf::basic::Singleton<HandlerManager>;
    using cs_handler_arr_type = std::array<BasicHandler*,msg::ENCSType_ARRAYSIZE>;
    using ss_handler_arr_type = std::array<BasicHandler*,msg::ENSSType_ARRAYSIZE>;

public:
    HandlerManager();
    static BasicHandler* Instance(Session const& session) { return base_type::Instance()->_Instance(session); }
    static BasicHandler* Instance(msg::CS const& message) { return base_type::Instance()->_Instance(message); }
    static BasicHandler* Instance(msg::SS const& message) { return base_type::Instance()->_Instance(message); }
    static bool IsRequest(msg::ENSSType msg_type) { return base_type::Instance()->_IsRequest(msg_type); }

    template<typename HandlerType>
    static void AddCSHandler(msg::ENCSType req_msg_type, msg::ENCSType rsp_msg_type = msg::CS_TYPE_NONE) {
        return base_type::Instance()->_AddCSHandler(req_msg_type, rsp_msg_type, new HandlerType());
    }

    template<typename HandlerType>
    static void AddSSHandler(msg::ENSSType req_msg_type, msg::ENSSType rsp_msg_type = msg::SS_TYPE_NONE) {
        return base_type::Instance()->_AddSSHandler(req_msg_type, rsp_msg_type, new HandlerType());
    }

private:
    BasicHandler* _Instance(Session const& session);
    BasicHandler* _Instance(msg::CS const& message);
    BasicHandler* _Instance(msg::SS const& message);
    bool _IsRequest(msg::ENSSType msg_type) { return _ss_handler_arr[msg_type] != nullptr; }

    void _AddCSHandler(msg::ENCSType req_msg_type, msg::ENCSType rsp_msg_type, BasicHandler* phandler) {
        _cs_handler_arr[req_msg_type] = phandler;
        _cs_handler_arr[req_msg_type]->SetMsgType(rsp_msg_type);
    }

    void _AddSSHandler(msg::ENSSType req_msg_type, msg::ENSSType rsp_msg_type, BasicHandler* phandler) {
        _ss_handler_arr[req_msg_type] = phandler;
        _ss_handler_arr[req_msg_type]->SetMsgType(rsp_msg_type);
    }


private:
    cs_handler_arr_type _cs_handler_arr;
    ss_handler_arr_type _ss_handler_arr;
    DefaultHandler _default_handler;
};


// vim:ts=4:sw=4:et:ft=cpp:
