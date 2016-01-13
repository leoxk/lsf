// File:        handler_common.cpp
// Description: ---
// Notes:       ---
// Author:      leoxiang <leoxiang727@qq.com>
// Revision:    2015-08-07 by leoxiang

#include "svr/common/config_manager.h"
#include "svr/datasvrd/handler_common.h"
#include "svr/datasvrd/leveldb_manager.h"

using namespace lsf::asio;
using namespace lsf::util;

#define MIN_LOCK_ID 1
#define MAX_LOCK_ID 65536

std::string ConvertDataKey(data::ENDataType type, uint64_t id) {
    char tmp[256];
    snprintf(tmp, sizeof(tmp), "%lu.%lu.dt", (size_t)type, id);
    return tmp;
}

std::string ConvertControlInfoKey(data::ENDataType type, uint64_t id) {
    char tmp[256];
    snprintf(tmp, sizeof(tmp), "%lu.%lu.ci", (size_t)type, id);
    return tmp;
}

void UpdateTimeoutLock(data::DBControlInfo& control_info) {
    if (!control_info.has_last_lock_id()) return;
    if (!control_info.has_last_lock_time()) return;
    if (control_info.last_lock_time() + ConfigManager::Instance()->ServerConfig().db_lock_timeout() >= IOService::Instance()->ClockTimeMilli()) return;

    control_info.clear_last_lock_time();
    control_info.clear_last_lock_id();
}

////////////////////////////////////////////////////////////
data::ENSessionState QueryDataHandler::OnServerRequest(Session& session) {
    auto& request = session.ss_request().query_data_req();
    auto& response = *session.mutable_ss_response()->mutable_query_data_rsp();
    response.set_type(request.type());
    response.set_id(request.id());

    // first get control info
    data::DBControlInfo control_info;
    auto control_info_key = ConvertControlInfoKey(request.type(), request.id());
    if (!LevelDBManager::Instance()->Query(control_info_key, control_info)) {
        response.set_result(msg::INTERNAL_ERROR_LEVELDB);
        return data::SESSION_STATE_FINISH;
    }
    UpdateTimeoutLock(control_info);
    response.set_version(control_info.version());

    // if need lock, try lock first
    if (request.need_lock()) {
        if (control_info.has_last_lock_id()) {
            response.set_result(msg::INTERNAL_ERROR_ALREADY_LOCK);
            return data::SESSION_STATE_FINISH;
        }
        // lock data
        control_info.set_last_lock_id(SingleRandom::Instance()->GetRand(MIN_LOCK_ID, MAX_LOCK_ID));
        control_info.set_last_lock_time(IOService::Instance()->ClockTimeMilli());
        response.set_lock_id(control_info.last_lock_id());
        // save control info
        if (!LevelDBManager::Instance()->Update(control_info_key, control_info)) {
            response.set_result(msg::INTERNAL_ERROR_LEVELDB);
            return data::SESSION_STATE_FINISH;
        }
    }

    // if has version, check already has newest data
    if (request.has_version() && request.version() == control_info.version()) {
        response.set_result(msg::INTERNAL_ERROR_OK);
        return data::SESSION_STATE_FINISH;
    }

    // if not, query data
    auto data_key = ConvertDataKey(request.type(), request.id());
    if (!LevelDBManager::Instance()->Query(data_key, *response.mutable_data())) {
        // here we do not release lock, because this is rare
        response.set_result(msg::INTERNAL_ERROR_LEVELDB);
        return data::SESSION_STATE_FINISH;
    }

    return data::SESSION_STATE_FINISH;
}

////////////////////////////////////////////////////////////
data::ENSessionState UpdateDataHandler::OnServerRequest(Session& session) {
    auto& request = session.ss_request().update_data_req();
    auto& response = *session.mutable_ss_response()->mutable_update_data_rsp();
    response.set_type(request.type());
    response.set_id(request.id());

    // first get control info
    data::DBControlInfo control_info;
    auto control_info_key = ConvertControlInfoKey(request.type(), request.id());
    if (!LevelDBManager::Instance()->Query(control_info_key, control_info)) {
        response.set_result(msg::INTERNAL_ERROR_LEVELDB);
        return data::SESSION_STATE_FINISH;
    }
    UpdateTimeoutLock(control_info);
    response.set_version(control_info.version());

    // check version match
    if (request.version() != control_info.version()) {
        response.set_result(msg::INTERNAL_ERROR_VERSION_NOT_MATCH);
        return data::SESSION_STATE_FINISH;
    }

    // check lock match
    if (request.lock_id() != control_info.last_lock_id()) {
        response.set_result(msg::INTERNAL_ERROR_LOCK_NOT_MATCH);
        return data::SESSION_STATE_FINISH;
    }

    // set info
    control_info.set_version(control_info.version() + 1);
    control_info.set_update_time(IOService::Instance()->ClockTime());
    control_info.clear_last_lock_id();
    control_info.clear_last_lock_time();

    // batch save
    std::vector<DBBatchInfo> vec;
    auto data_key = ConvertDataKey(request.type(), request.id());
    vec.emplace_back(control_info_key, &control_info);
    vec.emplace_back(data_key, &request.data());

    if (!LevelDBManager::Instance()->BatchUpdate(vec)) {
        response.set_result(msg::INTERNAL_ERROR_LEVELDB);
        return data::SESSION_STATE_FINISH;
    }

    return data::SESSION_STATE_FINISH;
}

////////////////////////////////////////////////////////////
data::ENSessionState DeleteDataHandler::OnServerRequest(Session& session) {
    auto& request = session.ss_request().delete_data_req();
    auto& response = *session.mutable_ss_response()->mutable_delete_data_rsp();
    response.set_type(request.type());
    response.set_id(request.id());

    auto control_info_key = ConvertControlInfoKey(request.type(), request.id());
    auto data_key = ConvertDataKey(request.type(), request.id());
    LevelDBManager::Instance()->Delete(control_info_key);
    LevelDBManager::Instance()->Delete(data_key);

    return data::SESSION_STATE_FINISH;
}

////////////////////////////////////////////////////////////
data::ENSessionState InsertDataHandler::OnServerRequest(Session& session) {
    auto& request = session.ss_request().insert_data_req();
    auto& response = *session.mutable_ss_response()->mutable_insert_data_rsp();
    response.set_type(request.type());
    response.set_id(request.id());

    // check exist
    auto control_info_key = ConvertControlInfoKey(request.type(), request.id());
    if (LevelDBManager::Instance()->Has(control_info_key)) {
        response.set_result(msg::INTERNAL_ERROR_ALREADY_HAS_DATA);
        return data::SESSION_STATE_FINISH;
    }

    // set info
    data::DBControlInfo control_info;
    control_info.set_version(1);
    control_info.set_update_time(IOService::Instance()->ClockTime());

    // batch save
    std::vector<DBBatchInfo> vec;
    auto data_key = ConvertDataKey(request.type(), request.id());
    vec.emplace_back(control_info_key, &control_info);
    vec.emplace_back(data_key, &request.data());

    if (!LevelDBManager::Instance()->BatchUpdate(vec)) {
        response.set_result(msg::INTERNAL_ERROR_LEVELDB);
        return data::SESSION_STATE_FINISH;
    }

    return data::SESSION_STATE_FINISH;
}

////////////////////////////////////////////////////////////
data::ENSessionState UnlockDataHandler::OnServerRequest(Session& session) {
    auto& request = *session.mutable_ss_request()->mutable_unlock_data_notify();

    // first get control info
    data::DBControlInfo control_info;
    auto control_info_key = ConvertControlInfoKey(request.type(), request.id());
    if (!LevelDBManager::Instance()->Query(control_info_key, control_info)) {
        return data::SESSION_STATE_FINISH;
    }
    UpdateTimeoutLock(control_info);

    // check own lock
    if (control_info.last_lock_id() != request.lock_id()) {
        return data::SESSION_STATE_FINISH;
    }

    // set unlock
    control_info.clear_last_lock_id();
    control_info.clear_last_lock_time();

    // save
    LevelDBManager::Instance()->Update(control_info_key, control_info);

    return data::SESSION_STATE_FINISH;
}

// vim:ts=4:sw=4:et:ft=cpp:
