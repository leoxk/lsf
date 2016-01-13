// File:        basic_manager.h
// Description: ---
// Notes:       ---
// Author:      leoxiang <leoxiang727@qq.com>
// Revision:    2015-06-15 by leoxiang

#pragma once
#include <unordered_map>
#include "lsf/container/shared_mem.hpp"
#include "svr/common/common_header.h"

template<typename ElemType>
class BasicManager : public lsf::basic::NonCopyable {
public:
    using key_type = typename ElemType::key_type;
    using value_type = ElemType;
    using map_type = std::unordered_map<key_type,value_type>;
    using iterator = typename map_type::iterator;
    constexpr static const float DEF_MAX_LOAD_FACTOR = 0.8;

public:
    ////////////////////////////////////////////////////////////
    // init mananger
    bool Init(key_t shm_key, size_t max_size) {
        // check input
        if (max_size == 0) {
            LSF_LOG_ERR_STACK("invalid input, %u", max_size);
            return false;
        }

        // save value
        _shm_key = shm_key;
        _max_size = max_size;

        // init hash
        _map.max_load_factor(DEF_MAX_LOAD_FACTOR);
        _map.reserve(max_size);

        // if find shm, then recover data
        if (_shm_key != 0 && lsf::container::SharedMem::IsShmExist(_shm_key)) {
            lsf::container::SharedMem shared_mem(_shm_key);
            uint8_t* pbase = (uint8_t*)shared_mem.GetPtr();
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
                    LSF_LOG_ERR_STACK("unserialize failed, count=%d, total=%u, %s", i, elem_size, LSF_PROTOBUF);
                    return false;
                }

                // insert into map
                _map[tmp.GetKey()] = std::move(tmp);
                _map[tmp.GetKey()].AfterUnSerialize();

            }

            // delete shm
            lsf::container::SharedMem::Delete(_shm_key);
        }

        // set status
        _is_init = true;

        return true;
    }

    ////////////////////////////////////////////////////////////
    // when manager released, serialize all data
    bool Release() {
        // just ignore empty shm key
        if (_shm_key == 0) return true;

        // if shm already exist, then delete it
        if (lsf::container::SharedMem::IsShmExist(_shm_key) && !lsf::container::SharedMem::Delete(_shm_key)) {
            LSF_LOG_ERR_STACK("delete shared mem failed, %s", LSF_SES());
            return false;
        }

        // count need byte size
        size_t byte_size = sizeof(size_t);
        for (auto const& elem : _map) {
            byte_size += elem.second.GetSize();
            byte_size += sizeof(size_t);
        }

        if (byte_size == 0) return true;

        // create shm mem
        if (!lsf::container::SharedMem::Create(_shm_key, byte_size)) {
            LSF_LOG_ERR_STACK("create shared mem failed, key=%x, byte_size=%u, %s", _shm_key, byte_size, LSF_SES());
            return false;
        }
        lsf::container::SharedMem shared_mem(_shm_key);
        if (!shared_mem.IsAttached()) {
            LSF_LOG_ERR_STACK("shared mem is not attached, key=%x", _shm_key);
            return false;
        }
        uint8_t* pbase = (uint8_t*)shared_mem.GetPtr();
        size_t total = shared_mem.GetSize();
        size_t off = 0;

        // serialize total count
        lsf::util::Serialize(pbase, total, off, (size_t)_map.size());

        // serialize every elem
        for (auto const& elem : _map) {
            if (!elem.second.Serialize(pbase, total, off)) {
                LSF_LOG_ERR_STACK("serialize failed, total=%u, off=%u, %s", total, off, LSF_PROTOBUF);
                LSF_LOG_ERR("\n%s", LSF_PTS(elem.second));
                return false;
            }
        }

        return true;
    }

public:
    bool IsInit() const { return _is_init; }
    value_type& operator[](key_type const& key) { return _map[key]; }
    void erase(key_type const& key) { _map.erase(key); }
    bool exist(key_type const& key) const { return _map.find(key) != _map.end(); }
    bool full() const { return _map.size() >= _max_size; }
    bool empty() const { return _map.empty(); }
    size_t size() const { return _map.size(); }
    size_t max_size() const { return _max_size; }
    iterator begin() { return _map.begin(); }
    iterator end() { return _map.end(); }
    iterator find(key_type const& key) { return _map.find(key); }

private:
    bool _is_init = false;
    map_type _map;
    key_t _shm_key;
    size_t _max_size;
};

// vim:ts=4:sw=4:et:ft=cpp:
