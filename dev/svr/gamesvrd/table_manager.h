// File:        table_manager.h
// Description: ---
// Notes:       ---
// Author:      leoxiang <leoxiang727@qq.com>
// Revision:    2015-07-13 by leoxiang

#pragma once
#include <set>
#include <map>
#include <tuple>
#include "svr/common/basic_manager.h"
#include "svr/common/common_proto.h"
#include "svr/common/timer_manager.h"
#include "svr/common/client_conn_manager.h"

////////////////////////////////////////////////////////////
// Table
class Table : public data::Table {
public:
    using base_type = data::Table;
    using key_type = decltype(base_type().table_id());

public:
    bool Serialize(void *buf, size_t buflen, size_t &uselen) const;
    bool UnSerialize(void *buf, size_t buflen, size_t &uselen);
    void AfterUnSerialize() { }
    size_t GetSize() const { return base_type::ByteSize(); }
    key_type GetKey() const { return base_type::table_id(); }
    std::string ToString() const;

public:
    void OnTableRoutineCheck();
};

////////////////////////////////////////////////////////////
// TableManager
class TableManager : public BasicManager<Table>, public lsf::basic::Singleton<TableManager> {
public:
    using base_type = BasicManager<Table>;
    using room_index_type = std::map<std::pair<conf::ENTableType,uint64_t>,std::set<uint32_t>>;
    static const size_t DEF_CHECK_INTERVAL = 10*1000;

public:
    bool Init(key_t shm_key, uint32_t max_size);
    Table* GetTable(uint32_t table_id);

protected:
    Table* CreateTable(conf::Table const& table_conf);
    void ReleaseTable(uint32_t table_id);

    std::set<uint32_t>& GetRoomIndex(conf::ENTableType type ,uint64_t sb) {
        return _room_index[std::make_pair(type, sb)];
    }

    std::set<uint32_t>& GetRoomIndex(Table const& table) {
        return GetRoomIndex(table.table_conf().table_type(), table.table_conf().small_blind());
    }

private:
    room_index_type _room_index;
};

// vim:ts=4:sw=4:et:ft=cpp:
