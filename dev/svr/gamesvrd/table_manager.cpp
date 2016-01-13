// File:        table_manager.cpp
// Description: ---
// Notes:       ---
// Author:      leoxiang <leoxiang727@qq.com>
// Revision:    2015-07-13 by leoxiang

#include "svr/common/common_header.h"
#include "svr/common/timer_manager.h"
#include "svr/common/basic_handler.h"
#include "svr/common/handler_manager.h"
#include "svr/common/config_manager.h"
#include "svr/common/common_func.h"
#include "svr/common/common_service.h"
#include "svr/common/client_conn_manager.h"
#include "svr/gamesvrd/table_manager.h"
#include "svr/gamesvrd/game_server.h"

using namespace lsf::basic;
using namespace lsf::util;
using namespace lsf::asio;

////////////////////////////////////////////////////////////
// Table
bool Table::Serialize(void *buf, size_t buflen, size_t& uselen) const {
    if (!lsf::util::SerializeProtobuf(buf, buflen, uselen, *this)) return false;
    return true;
}

bool Table::UnSerialize(void *buf, size_t buflen, size_t& uselen) {
    if (!lsf::util::UnSerializeProtobuf(buf, buflen, uselen, *this)) return false;
    return true;
}

std::string Table::ToString() const {
    static std::string tmp;
    tmp = "table_id=" + TypeCast<std::string>(base_type::table_id());
    return tmp.c_str();
}

////////////////////////////////////////////////////////////
// Table Manager
bool TableManager::Init(key_t shm_key, uint32_t max_size) {
    // call base init
    if (!base_type::Init(shm_key, max_size)) return false;

    // reinit room index
    for (auto& pair : *this) GetRoomIndex(pair.second).insert(pair.second.table_id());

    return true;
}

Table* TableManager::CreateTable(conf::Table const& table_conf) {
    // check full
    if (base_type::full()) {
        LSF_LOG_ERR("table is full, size=%u, max_size=%u", base_type::size(), base_type::max_size());
        return nullptr;
    }

    // create table
    Table& table = base_type::operator[](table_conf.table_id());

    // init
    table.set_table_id(table_conf.table_id());
    table.mutable_table_conf()->CopyFrom(table_conf);

    // add to index
    GetRoomIndex(table).insert(table.table_id());

    return &table;
}

void TableManager::ReleaseTable(uint32_t table_id) {
    // clear timer
    Table* ptable = GetTable(table_id);

    // erase index
    GetRoomIndex(*ptable).erase(ptable->table_id());

    // free object
    base_type::erase(table_id);
}

Table* TableManager::GetTable(uint32_t table_id) {
    auto iter = base_type::find(table_id);
    return iter == base_type::end() ? nullptr : &iter->second;
}

// vim:ts=4:sw=4:et:ft=cpp:
