// File:        leveldb_manager.h
// Description: ---
// Notes:       ---
// Author:      leoxiang <leoxiang727@qq.com>
// Revision:    2015-08-07 by leoxiang

#pragma once
#include <leveldb/db.h>
#include <leveldb/filter_policy.h>
#include <leveldb/cache.h>
#include <leveldb/write_batch.h>
#include "svr/common/common_header.h"
#include "svr/common/common_proto.h"
#include "svr/common/common_func.h"

struct DBBatchInfo {
    DBBatchInfo(std::string const& _key, std::string const* _pstring) : key(_key), pstring(_pstring) {}
    DBBatchInfo(std::string const& _key, google::protobuf::Message const* _pmessage) : key(_key), pmessage(_pmessage) {}

    std::string key;
    std::string const* pstring = nullptr;
    google::protobuf::Message const* pmessage = nullptr;
};

class LevelDBManager : public lsf::basic::Singleton<LevelDBManager> {
public:
    LevelDBManager() : _db(NULL) { }
    ~LevelDBManager() { delete _db; }

public:
    bool Init(size_t leveldb_cache, std::string const& leveldb_path);
    bool Has(std::string const& key);
    bool Query(std::string const& key, google::protobuf::Message& message);
    bool Query(std::string const& key, std::string& content);
    bool Update(std::string const& key, google::protobuf::Message const& message);
    bool Update(std::string const& key, std::string const& content);
    bool Delete(std::string const& key);

    bool BatchUpdate(std::vector<DBBatchInfo> const& batch_ops);

private:
    leveldb::DB* _db;
};


// vim:ts=4:sw=4:et:ft=cpp:
