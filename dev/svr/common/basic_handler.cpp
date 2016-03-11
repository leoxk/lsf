// File:        basic_handler.cpp
// Description: ---
// Notes:       ---
// Author:      leoxiang <leoxiang727@qq.com>
// Revision:    2015-07-07 by leoxiang

#include <algorithm>
#include "svr/common/basic_handler.h"
#include "svr/common/common_service.h"
#include "svr/common/common_func.h"
#include "svr/common/common_proto.h"
#include "svr/common/handler_manager.h"

using namespace lsf::asio;

////////////////////////////////////////////////////////////
// main logic
void BasicHandler::GetServerResponse(Session& session, msg::SS const& message) {
    session.mutable_svr_response()->CopyFrom(message);
    auto rsp_server_type = session.svr_response().proxy_head().src_server_type();
    conf::ENServerType need_rsp_server_type;
    data::ENSessionState session_state;
    switch (session.session_state()) {
        case data::SESSION_STATE_HTTPSVR_WAIT:
            need_rsp_server_type = conf::SERVER_TYPE_HTTP_SERVER;
            session_state = data::SESSION_STATE_HTTPSVR_MESSAGE;
            break;
        case data::SESSION_STATE_CONFSVR_WAIT:
            need_rsp_server_type = conf::SERVER_TYPE_CONFIG_CENTER;
            session_state = data::SESSION_STATE_CONFSVR_MESSAGE;
            break;
        case data::SESSION_STATE_DBSVR_QUERY_WAIT:
            need_rsp_server_type = conf::SERVER_TYPE_DATA_SERVER;
            session_state = data::SESSION_STATE_DBSVR_QUERY_MESSAGE;
            break;
        case data::SESSION_STATE_DBSVR_UPDATE_WAIT:
            need_rsp_server_type = conf::SERVER_TYPE_DATA_SERVER;
            session_state = data::SESSION_STATE_DBSVR_UPDATE_MESSAGE;
            break;
        case data::SESSION_STATE_DBSVR_DELETE_WAIT:
            need_rsp_server_type = conf::SERVER_TYPE_DATA_SERVER;
            session_state = data::SESSION_STATE_DBSVR_DELETE_MESSAGE;
            break;
        case data::SESSION_STATE_DBSVR_INSERT_WAIT:
            need_rsp_server_type = conf::SERVER_TYPE_DATA_SERVER;
            session_state = data::SESSION_STATE_DBSVR_INSERT_MESSAGE;
            break;
        default:
            LSF_LOG_FAT("invalid session state, %s", LSF_TS(session));
            return;
    }

    // check server type
    if (rsp_server_type != need_rsp_server_type)  {
        LSF_LOG_ERR("invalid server type, %s, %s, %s", LSF_ETS(rsp_server_type), LSF_ETS(need_rsp_server_type), LSF_TS(session));
        return;
    }

    // set state and run
    TimerManager::Instance()->ReleaseTimer(session.timer_id());
    session.clear_timer_id();
    session.mutable_svr_response()->CopyFrom(message);
    session.set_session_state(session_state);

    LSF_LOG_INF("get response from server, %s", LSF_TS(session));
    RunStateMachine(session);
}

void BasicHandler::WaitResponseTimeout(Session& session) {
    data::ENSessionState session_state;
    switch (session.session_state()) {
        case data::SESSION_STATE_HTTPSVR_WAIT:      session_state = data::SESSION_STATE_HTTPSVR_TIMEOUT; break;
        case data::SESSION_STATE_CONFSVR_WAIT:      session_state = data::SESSION_STATE_CONFSVR_TIMEOUT;  break;
        case data::SESSION_STATE_DBSVR_QUERY_WAIT:  session_state = data::SESSION_STATE_DBSVR_QUERY_TIMEOUT; break;
        case data::SESSION_STATE_DBSVR_UPDATE_WAIT: session_state = data::SESSION_STATE_DBSVR_UPDATE_TIMEOUT; break;
        case data::SESSION_STATE_DBSVR_DELETE_WAIT: session_state = data::SESSION_STATE_DBSVR_DELETE_TIMEOUT; break;
        case data::SESSION_STATE_DBSVR_INSERT_WAIT: session_state = data::SESSION_STATE_DBSVR_INSERT_TIMEOUT; break;
        case data::SESSION_STATE_CURL_WAIT:         session_state = data::SESSION_STATE_CURL_TIMEOUT; break;
        default:
            LSF_LOG_FAT("invalid session state, %s", LSF_TS(session));
            return;
    }

    // set state and run
    session.clear_timer_id();
    session.set_session_state(session_state);
    RunStateMachine(session);
}

void BasicHandler::GetClientRequest(Session& session, msg::CS const& message) {
    session.mutable_cs_request()->CopyFrom(message);
    session.set_session_state(data::SESSION_STATE_CLIENT_REQUEST);

    LSF_LOG_INF("get request from client, %s, msg_type=%s", LSF_TS(session), LSF_ETS(message.msg_type()));
    RunStateMachine(session);
}

void BasicHandler::GetServerRequest(Session& session, msg::SS const& message) {
    session.mutable_ss_request()->CopyFrom(message);
    session.set_session_state(data::SESSION_STATE_SERVER_REQUEST);

    LSF_LOG_INF("get request from server, %s, msg_type=%s", LSF_TS(session), LSF_ETS(message.msg_type()));
    RunStateMachine(session);
}

void BasicHandler::GetCurlResult(Session& session, bool result, std::string&& content) {
    LSF_LOG_INF("http response: session_id=%u, %s", session.session_id(), content.c_str());

    // set state and run
    TimerManager::Instance()->ReleaseTimer(session.timer_id());
    session.clear_timer_id();
    session.set_curl_result(result);
    session.set_curl_content(std::move(content));
    session.set_session_state(data::SESSION_STATE_CURL_MESSAGE);
    RunStateMachine(session);
}

////////////////////////////////////////////////////////////
// state machine
void BasicHandler::RunStateMachine(Session& session) {
    // state machine
    while (true) {
        // run state machine
        switch (session.session_state()) {
            // main logic loop
            case data::SESSION_STATE_CLIENT_REQUEST:       session.set_session_state(OnClientRequest(session));      break;
            case data::SESSION_STATE_SERVER_REQUEST:       session.set_session_state(OnServerRequest(session));      break;
            case data::SESSION_STATE_HTTPSVR_TIMEOUT:      session.set_session_state(OnHttpSvrTimeout(session));     break;
            case data::SESSION_STATE_HTTPSVR_MESSAGE:      session.set_session_state(OnHttpSvrMessage(session));     break;
            case data::SESSION_STATE_CONFSVR_TIMEOUT:      session.set_session_state(OnConfSvrTimeOut(session));     break;
            case data::SESSION_STATE_CONFSVR_MESSAGE:      session.set_session_state(OnConfSvrMessage(session));     break;
            case data::SESSION_STATE_CURL_MESSAGE:         session.set_session_state(OnCurlMessage(session));        break;
            case data::SESSION_STATE_CURL_TIMEOUT:         session.set_session_state(OnCurlTimeout(session));        break;
            case data::SESSION_STATE_DBSVR_NEXT:           session.set_session_state(OnDBSvrNext(session));          break;
            case data::SESSION_STATE_DBSVR_QUERY:          session.set_session_state(OnDBSvrQuery(session));         break;
            case data::SESSION_STATE_DBSVR_QUERY_TIMEOUT:  session.set_session_state(OnDBSvrQueryTimeout(session));  break;
            case data::SESSION_STATE_DBSVR_QUERY_MESSAGE:  session.set_session_state(OnDBSvrQueryMessage(session));  break;
            case data::SESSION_STATE_DBSVR_QUERY_SUCCESS:  session.set_session_state(OnDBSvrQuerySuccess(session));  break;
            case data::SESSION_STATE_DBSVR_UPDATE:         session.set_session_state(OnDBSvrUpdate(session));        break;
            case data::SESSION_STATE_DBSVR_UPDATE_TIMEOUT: session.set_session_state(OnDBSvrUpdateTimeout(session)); break;
            case data::SESSION_STATE_DBSVR_UPDATE_MESSAGE: session.set_session_state(OnDBSvrUpdateMessage(session)); break;
            case data::SESSION_STATE_DBSVR_UPDATE_SUCCESS: session.set_session_state(OnDBSvrUpdateSuccess(session)); break;
            case data::SESSION_STATE_DBSVR_DELETE:         session.set_session_state(OnDBSvrDelete(session));        break;
            case data::SESSION_STATE_DBSVR_DELETE_TIMEOUT: session.set_session_state(OnDBSvrDeleteTimeout(session)); break;
            case data::SESSION_STATE_DBSVR_DELETE_MESSAGE: session.set_session_state(OnDBSvrDeleteMessage(session)); break;
            case data::SESSION_STATE_DBSVR_DELETE_SUCCESS: session.set_session_state(OnDBSvrDeleteSuccess(session)); break;
            case data::SESSION_STATE_DBSVR_INSERT:         session.set_session_state(OnDBSvrInsert(session));        break;
            case data::SESSION_STATE_DBSVR_INSERT_TIMEOUT: session.set_session_state(OnDBSvrInsertTimeout(session)); break;
            case data::SESSION_STATE_DBSVR_INSERT_MESSAGE: session.set_session_state(OnDBSvrInsertMessage(session)); break;
            case data::SESSION_STATE_DBSVR_INSERT_SUCCESS: session.set_session_state(OnDBSvrInsertSuccess(session)); break;

            // set timer and exit loop
            case data::SESSION_STATE_HTTPSVR_WAIT:      SetSessionTimer(session, DEF_SESSION_HTTP_TIMEOUT); return;
            case data::SESSION_STATE_CONFSVR_WAIT:      SetSessionTimer(session, DEF_SESSION_TIMEOUT); return;
            case data::SESSION_STATE_DBSVR_QUERY_WAIT:  SetSessionTimer(session, DEF_SESSION_TIMEOUT); return;
            case data::SESSION_STATE_DBSVR_UPDATE_WAIT: SetSessionTimer(session, DEF_SESSION_TIMEOUT); return;
            case data::SESSION_STATE_DBSVR_DELETE_WAIT: SetSessionTimer(session, DEF_SESSION_TIMEOUT); return;
            case data::SESSION_STATE_DBSVR_INSERT_WAIT: SetSessionTimer(session, DEF_SESSION_TIMEOUT); return;
            case data::SESSION_STATE_CURL_WAIT:         SetSessionTimer(session, DEF_SESSION_HTTP_TIMEOUT); return;

            // end state machine
            case data::SESSION_STATE_FINISH:
                OnSessionFinish(session);
                SessionManager::Instance()->ReleaseSession(session.session_id());
                return;

            // error
            default:
                LSF_LOG_ERR("unknow session state, %s", LSF_TS(session));
                session.set_session_state(data::SESSION_STATE_FINISH);
                break;
        }
    }
}

void BasicHandler::AddQueryDataAndLock(Session& session, data::ENDataType data_type, uint64_t data_id) {
    AddQueryData(session, data_type, data_id);
    session.mutable_db_handles(session.db_handles_size()-1)->set_need_lock(true);
}

void BasicHandler::AddQueryData(Session& session, data::ENDataType data_type, uint64_t data_id) {
    // check duplicate
    for (auto& db_handle : session.db_handles()) {
        if (db_handle.type() == data_type && db_handle.id() == data_id) return;
    }
    // add new
    auto& db_handle = *session.add_db_handles();
    db_handle.set_type(data_type);
    db_handle.set_id(data_id);
}

void BasicHandler::AddDeleteData(Session& session, data::ENDataType data_type, uint64_t data_id) {
    // check duplicate
    for (auto& db_handle : session.db_handles()) {
        if (db_handle.type() == data_type && db_handle.id() == data_id) return;
    }
    // add new
    auto& db_handle = *session.add_db_handles();
    db_handle.set_type(data_type);
    db_handle.set_id(data_id);
}

data::DB* BasicHandler::AddInsertData(Session& session, data::ENDataType data_type, uint64_t data_id) {
    // check duplicate
    for (auto& db_handle : *session.mutable_db_handles()) {
        if (db_handle.type() == data_type && db_handle.id() == data_id) return db_handle.mutable_data();
    }
    // add new
    auto& db_handle = *session.add_db_handles();
    db_handle.set_type(data_type);
    db_handle.set_id(data_id);
    return db_handle.mutable_data();
}

////////////////////////////////////////////////////////////
// Default Handler
data::ENSessionState BasicHandler::OnClientRequest(Session& session) {
    // default query own data
    AddQueryDataAndLock(session, data::DATA_TYPE_USER_DATA, session.cs_request().conn_head().uid());
    return data::SESSION_STATE_DBSVR_QUERY;
}

data::ENSessionState BasicHandler::OnServerRequest(Session& session) {
    LSF_LOG_FAT("derived class should override this function, %s", LSF_TS(session));
    return data::SESSION_STATE_FINISH;
}

data::ENSessionState BasicHandler::OnHttpSvrMessage(Session& session) {
    LSF_LOG_FAT("derived class should override this function, %s", LSF_TS(session));
    return data::SESSION_STATE_FINISH;
}

data::ENSessionState BasicHandler::OnHttpSvrTimeout(Session& session) {
    LSF_LOG_ERR("httpsvr query timeout, %s", LSF_TS(session));
    SetErrorResponse(session, msg::ERROR_HTTPSVR_TIMEOUT);
    return data::SESSION_STATE_FINISH;
}

data::ENSessionState BasicHandler::OnConfSvrTimeOut(Session& session){
    LSF_LOG_ERR("confsvr query timeout, %s", LSF_TS(session));
    SetErrorResponse(session, msg::ERROR_CONFSVR_TIMEOUT);
    return data::SESSION_STATE_FINISH;
}

data::ENSessionState BasicHandler::OnConfSvrMessage(Session& session){
    LSF_LOG_FAT("derived class should override this function, %s", LSF_TS(session));
    return data::SESSION_STATE_FINISH;
}

data::ENSessionState BasicHandler::OnCurlMessage(Session& session) {
    LSF_LOG_FAT("derived class should override this function, %s", LSF_TS(session));
    return data::SESSION_STATE_FINISH;
}

data::ENSessionState BasicHandler::OnCurlTimeout(Session& session) {
    LSF_LOG_ERR("curl request timeout, %s", LSF_TS(session));
    SetErrorResponse(session, msg::INTERNAL_ERROR_CURL_TIMEOUT);
    return data::SESSION_STATE_FINISH;
}

data::ENSessionState BasicHandler::OnDBSvrNext(Session& session) {
    session.set_db_process_index(session.db_process_index() + 1);
    return session.db_process_index() >= (size_t)session.db_handles_size() ?
        data::SESSION_STATE_FINISH :
        data::SESSION_STATE_DBSVR_QUERY_SUCCESS;
}

////////////////////////////////////////////////////////////
// Query Process
bool IsQueryReady(Session& session) {
    for (auto& db_handle : session.db_handles()) {
        if (!db_handle.query_finish()) return false;
    }
    return true;
}

data::ENSessionState BasicHandler::OnDBSvrQuery(Session& session) {
    // check db process index
    if (session.db_process_index() >= (size_t)session.db_handles_size()) return data::SESSION_STATE_FINISH;

    // check input
    if (session.db_handles_size() == 0) {
        LSF_LOG_ERR("no query data, %s", LSF_TS(session));
        SetErrorResponse(session, msg::ERROR_DBSVR_QUERY);
        return data::SESSION_STATE_FINISH;
    }

    // query all
    for (auto& db_handle : session.db_handles()) {
        // query from db
        if (!QueryData(session, db_handle)) {
            SetErrorResponse(session, msg::ERROR_DBSVR_QUERY);
            return data::SESSION_STATE_FINISH;
        }
    }

    // check all data is ready
    if (IsQueryReady(session)) {
        session.set_db_process_index(0);
        return data::SESSION_STATE_DBSVR_QUERY_SUCCESS;
    }

    return data::SESSION_STATE_DBSVR_QUERY_WAIT;
}

data::ENSessionState BasicHandler::OnDBSvrQueryMessage(Session& session) {
    auto& response = session.svr_response().query_data_rsp();

    // check ret
    if (response.result() != msg::INTERNAL_ERROR_OK) {
        LSF_LOG_ERR("query data failed, %s, %s", LSF_ETS(response.result()), LSF_TS(session));
        SetErrorResponse(session, msg::ERROR_DBSVR_QUERY);
        return data::SESSION_STATE_FINISH;
    }

    // assign data
    for (auto& db_handle : *session.mutable_db_handles()) {
        if (db_handle.type() != response.type() || db_handle.id() != response.id()) continue;

        // set query finish
        db_handle.set_query_finish(true);

        // save lock id
        if (response.has_lock_id()) db_handle.set_lock_id(response.lock_id());

        // save version
        if (response.has_version()) db_handle.set_version(response.version());

        // save data
        switch (response.type()) {
            case data::DATA_TYPE_ROBOT_DATA: db_handle.mutable_data()->mutable_robot_data()->CopyFrom(response.data().robot_data()); break;
            default:
                LSF_LOG_ERR("unknow data type, %s", db_handle.type());
                SetErrorResponse(session, msg::ERROR_DBSVR_PARSE);
                return data::SESSION_STATE_FINISH;
        }

        // set data id
        switch (db_handle.type()) {
            case data::DATA_TYPE_ROBOT_DATA: db_handle.mutable_data()->mutable_robot_data()->set_uid(db_handle.id());   break;
            default: break;
        }
        break;
    }

    // check all data is ready
    if (IsQueryReady(session)) {
        session.set_db_process_index(0);
        return data::SESSION_STATE_DBSVR_QUERY_SUCCESS;
    }

    return data::SESSION_STATE_DBSVR_QUERY_WAIT;
}

data::ENSessionState BasicHandler::OnDBSvrQueryTimeout(Session& session) {
    LSF_LOG_ERR("dbsvr query timeout, %s", LSF_TS(session));
    SetErrorResponse(session, msg::ERROR_DBSVR_QUERY_TIMEOUT);
    return data::SESSION_STATE_FINISH;
}

data::ENSessionState BasicHandler::OnDBSvrQuerySuccess(Session& session) {
    LSF_LOG_FAT("derived class should override this function, %s", LSF_TS(session));
    return data::SESSION_STATE_FINISH;
}

////////////////////////////////////////////////////////////
// Update Process
data::ENSessionState BasicHandler::OnDBSvrUpdate(Session& session) {
    // check db process index
    if (session.db_process_index() >= (size_t)session.db_handles_size()) return data::SESSION_STATE_FINISH;

    msg::SS message;
    message.set_msg_type(msg::SS_TYPE_UPDATE_DATA_REQ);
    message.mutable_proxy_head()->set_session_id(session.session_id());
    auto& request  = *message.mutable_update_data_req();
    auto& db_handle = *session.mutable_db_handles(session.db_process_index());

    // set type and id
    request.set_type(db_handle.type());
    request.set_id(db_handle.id());
    request.set_version(db_handle.version());
    request.set_lock_id(db_handle.lock_id());

    // this is one time lock
    db_handle.clear_lock_id();

    // if no data need to save, just jump to update success
    switch (db_handle.type()) {
        case data::DATA_TYPE_ROBOT_DATA:
            if (db_handle.data().robot_data().ByteSize() == 0) return data::SESSION_STATE_DBSVR_UPDATE_SUCCESS;
            request.mutable_data()->mutable_robot_data()->CopyFrom(db_handle.data().robot_data());
            break;
        default:
            LSF_LOG_ERR("unknow data type, %s", db_handle.type());
            SetErrorResponse(session, msg::ERROR_DBSVR_UPDATE);
            return data::SESSION_STATE_FINISH;
    }

    // send to dbsvr
    ConnectServerMsgTransferService::Instance()->TransferMessageByHash(conf::SERVER_TYPE_DATA_SERVER, request.id(), message);

    return data::SESSION_STATE_DBSVR_UPDATE_WAIT;
}

data::ENSessionState BasicHandler::OnDBSvrUpdateMessage(Session& session) {
    auto& response = session.svr_response().update_data_rsp();

    // check type and id
    auto& db_handle = *session.mutable_db_handles(session.db_process_index());
    if (db_handle.type() != response.type() || db_handle.id() != response.id()) {
        LSF_LOG_FAT("db update type and id not match, %s|%s, %u|%u, %s", LSF_ETS(db_handle.type()),
                LSF_ETS(response.type()), db_handle.id(), response.id(), LSF_TS(session));
        SetErrorResponse(session, msg::ERROR_DBSVR_UPDATE);
        return data::SESSION_STATE_FINISH;
    }

    // check ret
    if (response.result() != msg::INTERNAL_ERROR_OK) {
        LSF_LOG_ERR("update data failed, %s, %s", LSF_ETS(response.result()), LSF_TS(session));
        SetErrorResponse(session, msg::ERROR_DBSVR_UPDATE);
        return data::SESSION_STATE_FINISH;
    }

    return data::SESSION_STATE_DBSVR_UPDATE_SUCCESS;
}

data::ENSessionState BasicHandler::OnDBSvrUpdateTimeout(Session& session) {
    LSF_LOG_ERR("dbsvr update timeout, %s", LSF_TS(session));
    SetErrorResponse(session, msg::ERROR_DBSVR_UPDATE_TIMEOUT);
    return data::SESSION_STATE_FINISH;
}

data::ENSessionState BasicHandler::OnDBSvrUpdateSuccess(Session& session) {
    // default end session
    return data::SESSION_STATE_DBSVR_NEXT;
}

////////////////////////////////////////////////////////////
// Insert Process
data::ENSessionState BasicHandler::OnDBSvrInsert(Session& session) {
    // check db process index
    if (session.db_process_index() >= (size_t)session.db_handles_size()) return data::SESSION_STATE_FINISH;

    // get db query
    auto& db_handle = session.db_handles(session.db_process_index());

    // construct request
    msg::SS message;
    message.set_msg_type(msg::SS_TYPE_INSERT_DATA_REQ);
    message.mutable_proxy_head()->set_session_id(session.session_id());
    auto& request  = *message.mutable_insert_data_req();
    request.set_type(db_handle.type());
    request.set_id(db_handle.id());
    request.mutable_data()->CopyFrom(db_handle.data());
    ConnectServerMsgTransferService::Instance()->TransferMessageByHash(conf::SERVER_TYPE_DATA_SERVER, db_handle.id(), message);

    return data::SESSION_STATE_DBSVR_INSERT_WAIT;
}

data::ENSessionState BasicHandler::OnDBSvrInsertMessage(Session& session) {
    auto& response = session.svr_response().insert_data_rsp();

    // check type and id
    auto& db_handle = *session.mutable_db_handles(session.db_process_index());
    if (db_handle.type() != response.type() || db_handle.id() != response.id()) {
        LSF_LOG_FAT("db insert type and id not match, %s|%s, %u|%u, %s", LSF_ETS(db_handle.type()),
                LSF_ETS(response.type()), db_handle.id(), response.id(), LSF_TS(session));
        SetErrorResponse(session, msg::ERROR_DBSVR_INSERT);
        return data::SESSION_STATE_FINISH;
    }

    // check ret
    if (response.result() != msg::INTERNAL_ERROR_OK) {
        LSF_LOG_ERR("insert data failed, %s, %s", LSF_ETS(response.result()), LSF_TS(session));
        SetErrorResponse(session, msg::ERROR_DBSVR_INSERT);
        return data::SESSION_STATE_FINISH;
    }

    return data::SESSION_STATE_DBSVR_INSERT_SUCCESS;
}

data::ENSessionState BasicHandler::OnDBSvrInsertTimeout(Session& session) {
    LSF_LOG_ERR("dbsvr insert timeout, %s", LSF_TS(session));
    SetErrorResponse(session, msg::ERROR_DBSVR_INSERT_TIMEOUT);
    return data::SESSION_STATE_FINISH;
}

data::ENSessionState BasicHandler::OnDBSvrInsertSuccess(Session& session) {
    return data::SESSION_STATE_FINISH;
}

////////////////////////////////////////////////////////////
// Delete Process
data::ENSessionState BasicHandler::OnDBSvrDelete(Session& session) {
    // check db process index
    if (session.db_process_index() >= (size_t)session.db_handles_size()) return data::SESSION_STATE_FINISH;

    // get db query
    auto& db_handle = session.db_handles(session.db_process_index());

    // construct request
    msg::SS message;
    message.set_msg_type(msg::SS_TYPE_DELETE_DATA_REQ);
    message.mutable_proxy_head()->set_session_id(session.session_id());
    auto& request  = *message.mutable_delete_data_req();
    request.set_type(db_handle.type());
    request.set_id(db_handle.id());
    ConnectServerMsgTransferService::Instance()->TransferMessageByHash(conf::SERVER_TYPE_DATA_SERVER, db_handle.id(), message);

    return data::SESSION_STATE_DBSVR_DELETE_WAIT;
}

data::ENSessionState BasicHandler::OnDBSvrDeleteMessage(Session& session) {
    auto& response = session.svr_response().delete_data_rsp();

    // check type and id
    auto& db_handle = *session.mutable_db_handles(session.db_process_index());
    if (db_handle.type() != response.type() || db_handle.id() != response.id()) {
        LSF_LOG_FAT("db delete type and id not match, %s|%s, %u|%u, %s", LSF_ETS(db_handle.type()),
                LSF_ETS(response.type()), db_handle.id(), response.id(), LSF_TS(session));
        SetErrorResponse(session, msg::ERROR_DBSVR_DELETE);
        return data::SESSION_STATE_FINISH;
    }

    // check ret
    if (response.result() != msg::INTERNAL_ERROR_OK) {
        LSF_LOG_ERR("delete data failed, %s, %s", LSF_ETS(response.result()), LSF_TS(session));
        SetErrorResponse(session, msg::ERROR_DBSVR_DELETE);
        return data::SESSION_STATE_FINISH;
    }

    return data::SESSION_STATE_DBSVR_DELETE_SUCCESS;
}

data::ENSessionState BasicHandler::OnDBSvrDeleteTimeout(Session& session) {
    LSF_LOG_ERR("dbsvr delete timeout, %s", LSF_TS(session));
    SetErrorResponse(session, msg::ERROR_DBSVR_DELETE_TIMEOUT);
    return data::SESSION_STATE_FINISH;
}

data::ENSessionState BasicHandler::OnDBSvrDeleteSuccess(Session& session) {
    return data::SESSION_STATE_FINISH;
}

void BasicHandler::OnSessionFinish(Session& session) {
    // unlock all data
    for (auto& db_handle : session.db_handles()) {
        if (db_handle.need_lock() && db_handle.has_lock_id()) {
            msg::SS message;
            message.set_msg_type(msg::SS_TYPE_UNLOCK_DATA_NOTIFY);
            message.mutable_unlock_data_notify()->set_type(db_handle.type());
            message.mutable_unlock_data_notify()->set_id(db_handle.id());
            message.mutable_unlock_data_notify()->set_lock_id(db_handle.lock_id());
            ConnectServerMsgTransferService::Instance()->TransferMessageByHash(conf::SERVER_TYPE_DATA_SERVER, db_handle.id(), message);
        }
    }

    SendResponse(session);
}

////////////////////////////////////////////////////////////
// Private Func
bool BasicHandler::QueryData(Session& session, data::DBHandle const& db_handle) {
    // construct request and send
    msg::SS message;
    message.set_msg_type(msg::SS_TYPE_QUERY_DATA_REQ);
    message.mutable_proxy_head()->set_session_id(session.session_id());
    message.mutable_query_data_req()->set_type(db_handle.type());
    message.mutable_query_data_req()->set_id(db_handle.id());
    if (db_handle.has_version()) message.mutable_query_data_req()->set_version(db_handle.version());
    if (db_handle.has_need_lock()) message.mutable_query_data_req()->set_need_lock(db_handle.need_lock());
    ConnectServerMsgTransferService::Instance()->TransferMessageByHash(conf::SERVER_TYPE_DATA_SERVER, db_handle.id(), message);
    return true;
}

bool BasicHandler::SetErrorResponse(Session& session, msg::ENError error) {
    if (!session.cs_request().has_msg_type() || _cs_rsp_type == msg::CS_TYPE_NONE) return false;
    common::SetResponse(session.cs_request(), *session.mutable_cs_response(), error);
    return true;
}

bool BasicHandler::SetErrorResponse(Session& session, msg::ENInternalError error) {
    if (!session.ss_request().has_msg_type() || _ss_rsp_type == msg::SS_TYPE_NONE) return false;
    common::SetResponse(session.ss_request(), *session.mutable_ss_response(), error);
    return true;
}

bool BasicHandler::SendResponse(Session& session) {
    if (session.is_send_rsp()) return true;

    if (session.cs_request().has_msg_type() && _cs_rsp_type != msg::CS_TYPE_NONE) {
        session.set_is_send_rsp(true);
        session.mutable_cs_response()->set_msg_type(_cs_rsp_type);
        return ConnectClientMsgTransferService::Instance()->TransferResponse(session);
    }

    if (session.ss_request().has_msg_type() && _ss_rsp_type != msg::SS_TYPE_NONE) {
        session.set_is_send_rsp(true);
        session.mutable_ss_response()->set_msg_type(_ss_rsp_type);
        return ConnectServerMsgTransferService::Instance()->TransferResponse(session);
    }

    return true;
}

bool BasicHandler::SetSessionTimer(Session& session, uint64_t milli_second) {
    Timer* ptimer = TimerManager::Instance()->CreateTimer(milli_second, data::TIMER_TYPE_SESSION_TIMEOUT);
    if (ptimer) {
        ptimer->set_session_id(session.session_id());
        session.set_timer_id(ptimer->timer_id());
        return true;
    }
    return false;
}

bool BasicHandler::RequestUrl(Session& session, std::string const& url, std::string const& body) {
    // set url
    SharedCurl shared_curl;
    shared_curl->SetUrl(url);
    LSF_LOG_INF("http request: session_id=%u, %s", session.session_id(), url.c_str());

    // post data
    if (!body.empty()) {
        if (!shared_curl->SetPostData(body)) {
            LSF_LOG_ERR("set post data failed, %s, %s", shared_curl->ErrString().c_str(), body.c_str());
            return false;
        }
        LSF_LOG_INF("http body: session_id=%u, %s", session.session_id(), body.c_str());
    }

    // add to multi handle
    return CurlMulti::Instance()->AddCurl(shared_curl, std::bind([](SharedCurl shared_curl, uint32_t session_id) {
        // get session
        auto* psession = SessionManager::Instance()->GetSession(session_id);
        if (psession == nullptr) {
            LSF_LOG_ERR("cant find session when curl finish, session_id=%u", session_id);
            return;
        }

        // check result
        if (!shared_curl->Success()) {
            LSF_LOG_ERR("http reuqest failed, session_id=%u, %s, %s", session_id, shared_curl->EffectiveUrl().c_str(), shared_curl->ErrString().c_str());
        }

        // process
        HandlerManager::Instance(*psession)->GetCurlResult(*psession, shared_curl->Success(), shared_curl->MoveOutput());
    }, std::placeholders::_1, session.session_id()));
}


// vim:ts=4:sw=4:et:ft=cpp:
