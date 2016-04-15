// File:        basic_handler.h
// Description: ---
// Notes:       ---
// Author:      leoxiang <leoxiang727@qq.com>
// Revision:    2015-07-07 by leoxiang

#pragma once
#include <set>
#include "svr/common/common_header.h"
#include "svr/common/session_manager.h"
#include "svr/common/common_service.h"
#include "svr/common/common_proto.h"

////////////////////////////////////////////////////////////
// BasicHandler
class BasicHandler {
public:
    friend lsf::basic::TestCase;
    static const size_t MAX_REDO_COUNT = 3;
    static const size_t DEF_SESSION_TIMEOUT = 2 * 1000;
    static const size_t DEF_SESSION_HTTP_TIMEOUT = 10 * 1000;
    static const size_t DEF_SESSION_CURL_TIMEOUT = 5 * 1000;

public:
    virtual ~BasicHandler() { }
    void SetMsgType(msg::ENCSType msg_type) { _cs_rsp_type = msg_type; }
    void SetMsgType(msg::ENSSType msg_type) { _ss_rsp_type = msg_type; }

public:
    // main logic
    void GetServerResponse(Session& session, msg::SS const& message);
    void WaitResponseTimeout(Session& session);
    void GetClientRequest(Session& session, msg::CS const& message);
    void GetServerRequest(Session& session, msg::SS const& message);
    void GetCurlResult(Session& session, bool result, std::string&& content);

protected:
    // state machine
    void RunStateMachine(Session& session);
    virtual data::ENSessionState OnClientRequest(Session& session);
    virtual data::ENSessionState OnServerRequest(Session& session);
    virtual data::ENSessionState OnHttpSvrMessage(Session& session);
    virtual data::ENSessionState OnHttpSvrTimeout(Session& session);
    virtual data::ENSessionState OnConfSvrMessage(Session& session);
    virtual data::ENSessionState OnConfSvrTimeOut(Session& session);
    virtual data::ENSessionState OnCurlMessage(Session& session);
    virtual data::ENSessionState OnCurlTimeout(Session& session);
    virtual data::ENSessionState OnDBSvrNext(Session& session);
    virtual data::ENSessionState OnDBSvrQuery(Session& session);
    virtual data::ENSessionState OnDBSvrQueryMessage(Session& session);
    virtual data::ENSessionState OnDBSvrQueryTimeout(Session& session);
    virtual data::ENSessionState OnDBSvrQuerySuccess(Session& session);
    virtual data::ENSessionState OnDBSvrUpdate(Session& session);
    virtual data::ENSessionState OnDBSvrUpdateMessage(Session& session);
    virtual data::ENSessionState OnDBSvrUpdateTimeout(Session& session);
    virtual data::ENSessionState OnDBSvrUpdateSuccess(Session& session);
    virtual data::ENSessionState OnDBSvrInsert(Session& session);
    virtual data::ENSessionState OnDBSvrInsertMessage(Session& session);
    virtual data::ENSessionState OnDBSvrInsertTimeout(Session& session);
    virtual data::ENSessionState OnDBSvrInsertSuccess(Session& session);
    virtual data::ENSessionState OnDBSvrDelete(Session& session);
    virtual data::ENSessionState OnDBSvrDeleteMessage(Session& session);
    virtual data::ENSessionState OnDBSvrDeleteTimeout(Session& session);
    virtual data::ENSessionState OnDBSvrDeleteSuccess(Session& session);
    virtual void OnSessionFinish(Session& session);

protected:
    void AddQueryData(Session& session, data::ENDataType data_type, uint64_t data_id);
    void AddQueryDataAndLock(Session& session, data::ENDataType data_type, uint64_t data_id);
    void AddDeleteData(Session& session, data::ENDataType data_type, uint64_t data_id);
    data::DB* AddInsertData(Session& session, data::ENDataType data_type, uint64_t data_id);
    bool SendResponse(Session& session);

    // must followed by return data::SESSION_STATE_CURL_WAIT;
    bool RequestUrl(Session& session, std::string const& url, std::string const& body = "");

private:
    bool QueryData(Session& session, data::DBHandle const& db_handle);
    bool SetErrorResponse(Session& session, msg::ENError error);
    bool SetErrorResponse(Session& session, msg::ENInternalError error);
    bool SetSessionTimer(Session& session, uint64_t milli_second);

protected:
    msg::ENCSType _cs_rsp_type = msg::CS_TYPE_NONE;
    msg::ENSSType _ss_rsp_type = msg::SS_TYPE_NONE;
};

// vim:ts=4:sw=4:et:ft=cpp:
