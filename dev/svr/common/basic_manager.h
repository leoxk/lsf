// File:        basic_manager.h
// Description: ---
// Notes:       ---
// Author:      leoxiang <leoxiang727@qq.com>
// Revision:    2015-06-15 by leoxiang

#pragma once

#include <map>
#include "lsf/container/pool.hpp"
#include "lsf/container/heap_mem.hpp"
#include "lsf/basic/singleton.hpp"
#include "lsf/basic/noncopyable.hpp"
#include "lsf/util/protobuf_log.hpp"
#include "lsf/util/serialize.hpp"
#include "lsf/util/log.hpp"
#include "lsf/util/time.hpp"

template <typename ElemType>
class BasicManager : public lsf::basic::NonCopyable {
public:
    using key_type = typename ElemType::key_type;
    using value_type = ElemType;
    using map_type = std::map<key_type,value_type>;
    using iterator = typename map_type::iterator;
    constexpr static const float DEF_LOAD_FACTOR = 0.9;

public:
    ////////////////////////////////////////////////////////////
    // init mananger
    bool Init(key_t shm_key, size_t max_size, float load_factor = DEF_LOAD_FACTOR) {
        // check input
        if (shm_key == 0 || max_size == 0) {
            LSF_LOG_ERR("invalid input, %u, %u", shm_key, max_size);
            return false;
        }

        // save value
        _shm_key = shm_key;
        _max_size = max_size;

        // if find shm, then recover data
        lsf::container::SharedMem shared_mem(_shm_key);
        if (shared_mem.IsAttached()) {
            uint8_t *pbase = (uint8_t *)shared_mem.GetPtr();
            size_t total = shared_mem.GetSize();
            size_t off = 0;

            // get total count
            size_t elem_size = 0;
            lsf::util::UnSerialize(pbase, total, off, elem_size);

            // recover every elem
            for (size_t i = 0; i < elem_size && i < max_size; ++i) {
                // recover from buffer
                value_type tmp;
                if (!tmp.UnSerialize(pbase, total, off)) {
                    LSF_LOG_ERR("unserialize failed, count=%d, %s", i,
                                lsf::util::ProtobufLog::Instance()->ErrCharStr());
                    return false;
                }

                // insert into map
                _map[tmp.GetKey()] = std::move(tmp);
            }

            // delete shm
            lsf::container::SharedMem::Delete(_shm_key);
        }

        return true;
    }

    ////////////////////////////////////////////////////////////
    // when manager released, serialize all data
    bool Release() {
        // if shm already exist, then delete it
        if (lsf::container::SharedMem::IsShmExist(_shm_key) && !lsf::container::SharedMem::Delete(_shm_key)) {
            LSF_LOG_ERR("delete shared mem failed, %s", lsf::container::SharedMem::SysErrCharStr());
            return false;
        }

        // count need byte size
        size_t byte_size = 0;
        for (auto const & elem : _map) byte_size += elem.GetSize();

        // create shm mem
        if (!lsf::container::SharedMem::Create(_shm_key, byte_size)) {
            LSF_LOG_ERR("create shared mem failed, key=%x, byte_size=%u, %s", _shm_key, byte_size,
                        lsf::container::SharedMem::SysErrCharStr());
            return false;
        }
        lsf::container::SharedMem shared_mem(_shm_key);
        if (!shared_mem.IsAttached()) {
            LSF_LOG_ERR("shared mem is not attached, key=%x", _shm_key);
            return false;
        }
        uint8_t *pbase = (uint8_t *)shared_mem.GetPtr();
        size_t total = shared_mem.GetSize();
        size_t off = 0;

        // serialize total count
        lsf::util::Serialize(pbase, total, off, (size_t)_map.size());

        // serialize every elem
        for (auto const & elem : _map) {
            if (!elem.Serialize(pbase, total, off)) {
                LSF_LOG_ERR("serialize failed, total=%u, off=%u, %s", total, off,
                            lsf::util::ProtobufLog::Instance()->ErrCharStr());
                return false;
            }
        }

        return true;
    }

public:
    value_type & operator[](key_type const & key) { return _map[key]; }
    void erase(key_type const& key) { _map.erase(key); }
    bool exist(key_type const& key) const { return _map.find(key) != _map.end(); }
    bool full() const { return _map.size() == _max_size; }
    bool empty() const { return _map.empty(); }
    size_t size() const { return _map.size(); }
    size_t max_size() const { return _max_size; }
    iterator begin() { return _map.begin(); }
    iterator end() { return _map.end(); }
    iterator find(key_type const& key) { return _map.find(key); }

private:
    map_type _map;
    key_t _shm_key;
    size_t _max_size;
};

// vim:ts=4:sw=4:et:ft=cpp:
