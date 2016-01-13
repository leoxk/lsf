// File:        leveldb_manager.cpp
// Description: ---
// Notes:       ---
// Author:      leoxiang <leoxiang727@qq.com>
// Revision:    2015-08-07 by leoxiang

#include "svr/datasvrd/leveldb_manager.h"

using namespace lsf::basic;
using namespace lsf::util;

bool LevelDBManager::Init(size_t leveldb_cache, std::string const& leveldb_path) {
    leveldb::Options options;
    options.create_if_missing = true;
    options.filter_policy = leveldb::NewBloomFilterPolicy(10);

    if (leveldb_cache != 0) {
        options.block_cache = leveldb::NewLRUCache(leveldb_cache * 1024 * 1024);
    }

    // for make directory
    if (!System::MkDir(leveldb_path)) {
        LSF_LOG_ERR("[LEVELDB] cant create dir, path=%s, %s", leveldb_path.c_str(), LSF_SES());
        return false;
    }

    // init leveldb
    leveldb::Status status = leveldb::DB::Open(options, leveldb_path, &_db);
    if (!status.ok()) {
        LSF_LOG_ERR("[LEVELDB] create leveldb failed, %s", LSF_TS(status));
        return false;
    }

    return true;
}

bool LevelDBManager::Has(std::string const& key) {
    std::string content;
    leveldb::Status status = _db->Get(leveldb::ReadOptions(), key, &content);
    return status.ok();
}

bool LevelDBManager::Query(std::string const& key, google::protobuf::Message& message) {
    // read data
    std::string content;
    leveldb::Status status = _db->Get(leveldb::ReadOptions(), key, &content);
    if (!status.ok() && !status.IsNotFound()) {
        LSF_LOG_ERR("[LEVELDB] query data failed, %s, key=%s", LSF_TS(status), key.c_str());
        return false;
    }

    if (status.ok()) {
        if (!Protobuf::ParseProtoMsgFromString(content, message)) {
            LSF_LOG_ERR("[LEVELDB] parse data failed, key=%s, %s", key.c_str(), LSF_PROTOBUF);
            LSF_LOG_ERR("[LEVELDB] content: %s", StringExt::BinToHexString(content).c_str());
            return false;
        }
    }

    return true;
}

bool LevelDBManager::Query(std::string const& key, std::string& content) {
    // read data
    content.clear();
    leveldb::Status status = _db->Get(leveldb::ReadOptions(), key, &content);
    if (!status.ok() && !status.IsNotFound()) {
        LSF_LOG_ERR("[LEVELDB] query data failed, %s, key=%s", LSF_TS(status), key.c_str());
        return false;
    }

    return true;
}

bool LevelDBManager::Update(std::string const& key, google::protobuf::Message const& message) {
    // serialize data
    std::string content;
    if (!Protobuf::SerializeProtoMsgToString(content, message)) {
        LSF_LOG_ERR("[LEVELDB] serialize data failed, key=%s, %s", key.c_str(), LSF_PROTOBUF);
        return false;
    }

    // update data
    leveldb::Status status = _db->Put(leveldb::WriteOptions(), key, content);
    if (!status.ok()) {
        LSF_LOG_ERR("[LEVELDB] update data failed, %s, key=%s", LSF_TS(status), key.c_str());
        return false;
    }

    return true;
}

bool LevelDBManager::Update(std::string const& key, std::string const& content) {
    // update data
    leveldb::Status status = _db->Put(leveldb::WriteOptions(), key, content);
    if (!status.ok()) {
        LSF_LOG_ERR("[LEVELDB] update data failed, %s, key=%s", LSF_TS(status), key.c_str());
        return false;
    }

    return true;
}

bool LevelDBManager::Delete(std::string const& key) {
    // delete data
    leveldb::Status status = _db->Delete(leveldb::WriteOptions(), key);
    if (!status.ok()) {
        LSF_LOG_ERR("[LEVELDB] delete data failed, %s, key=%s", LSF_TS(status), key.c_str());
        return false;
    }

    return true;
}

bool LevelDBManager::BatchUpdate(std::vector<DBBatchInfo> const& batch_ops) {
    // batch update
    leveldb::WriteBatch batch;
    for (auto& op : batch_ops) {
        // use string
        if (op.pstring != nullptr) {
            batch.Put(op.key, *op.pstring);
            continue;
        }

        // use message
        if (op.pmessage != nullptr) {
            std::string content;
            if (!Protobuf::SerializeProtoMsgToString(content, *op.pmessage)) {
                LSF_LOG_ERR("[LEVELDB] serialize data failed, key=%s, %s", op.key.c_str(), LSF_PROTOBUF);
                return false;
            }

            LSF_LOG_INF("[LEVELDB] save: %s, %s", op.key.c_str(), StringExt::BinToHexString(content).c_str());
            batch.Put(op.key, content);
            continue;
        }

        // else error
        LSF_LOG_ERR("[LEVELDB] data has no string nor message, key=%s", op.key.c_str());
    }

    // do batch update
    leveldb::Status status = _db->Write(leveldb::WriteOptions(), &batch);
    if (!status.ok()) {
        LSF_LOG_ERR("[LEVELDB] batch update data failed, %s", LSF_TS(status));
        return false;
    }

    return true;
}


// vim:ts=4:sw=4:et:ft=cpp:
