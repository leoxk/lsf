// File:        handler_manager.cpp
// Description: ---
// Notes:       ---
// Author:      leoxiang <leoxiang727@qq.com>
// Revision:    2015-07-07 by leoxiang

#include "svr/common/handler_manager.h"

////////////////////////////////////////////////////////////
// DefaultHandler
data::ENSessionState DefaultHandler::OnClientRequest(Session& session) {
    LSF_LOG_FAT("drived class should override this function, %s", LSF_TS(session));
    return data::SESSION_STATE_FINISH;
}

data::ENSessionState DefaultHandler::OnServerRequest(Session& session) {
    LSF_LOG_FAT("drived class should override this function, %s", LSF_TS(session));
    return data::SESSION_STATE_FINISH;
}

////////////////////////////////////////////////////////////
// HandlerManager
HandlerManager::HandlerManager() {
    // init
    _cs_handler_arr.fill(nullptr);
    _ss_handler_arr.fill(nullptr);
}

BasicHandler* HandlerManager::_Instance(Session const& session) {
    // handle cs request
    if (session.cs_request().has_msg_type()) return _Instance(session.cs_request());

    // handle ss request
    if (session.ss_request().has_msg_type()) return _Instance(session.ss_request());

    LSF_LOG_FAT_STACK("session not have any msg type, %s", LSF_TS(session));
    return &_default_handler;
}

BasicHandler* HandlerManager::_Instance(msg::CS const& message) {
    if (!msg::ENCSType_IsValid(message.msg_type())) {
        LSF_LOG_FAT("invalid msg type, %s", LSF_ETS(message.msg_type()));
        return &_default_handler;
    }
    if (_cs_handler_arr[message.msg_type()] == nullptr) {
        LSF_LOG_FAT("msg type not register, %s", LSF_ETS(message.msg_type()));
        return &_default_handler;
    }
    return _cs_handler_arr[message.msg_type()];
}

BasicHandler* HandlerManager::_Instance(msg::SS const& message) {
    if (!msg::ENSSType_IsValid(message.msg_type())) {
        LSF_LOG_FAT("invalid msg type, %s", LSF_ETS(message.msg_type()));
        return &_default_handler;
    }
    if (_ss_handler_arr[message.msg_type()] == nullptr) {
        LSF_LOG_FAT("msg type not register, %s", LSF_ETS(message.msg_type()));
        return &_default_handler;
    }
    return _ss_handler_arr[message.msg_type()];
}

// vim:ts=4:sw=4:et:ft=cpp:
