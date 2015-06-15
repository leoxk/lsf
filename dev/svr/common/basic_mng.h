// File:        basic_mng.h
// Description: ---
// Notes:       ---
// Author:      leoxiang <leoxiang727@qq.com>
// Revision:    2015-06-15 by leoxiang

#pragma once

#include <sys/time.h>
#include <set>
#include <unordered_map>
#include "lsf/container/pool.hpp"
#include "lsf/container/heap_mem.hpp"
#include "lsf/basic/singleton.hpp"
#include "lsf/basic/noncopyable.hpp"
#include "lsf/util/protobuf_log.hpp"
#include "lsf/util/serialize.hpp"
#include "lsf/util/log.hpp"

template<typename ElemType>
class BasicManager : public lsf::basic::NonCopyable
{
public:
    typedef lsf::container::Pool<ElemType,lsf::container::HeapMem,uint32_t> pool_type;
    typedef typename pool_type::iterator iterator;

public:
    ////////////////////////////////////////////////////////////
    // init mananger
    bool Init(key_t shm_key, uint32_t max_size)
    {
        // save shm key
        _shm_key = shm_key;

        // calc shm size and bind
        size_t byte_size = _pool.CalcByteSize(max_size);
        if (!_pool.BindAndInitStorage(lsf::container::HeapMem(byte_size)))
        {
            LSF_LOG_ERR("bind and init storage failed, %s", _pool.ErrCharStr());
            return false;
        }

        // if find shm, then recover data
        lsf::container::SharedMem shared_mem(_shm_key);
        if (shared_mem.IsAttached())
        {
            uint8_t * pbase = (uint8_t *)shared_mem.GetPtr();
            size_t total = shared_mem.GetSize();
            size_t off = 0;

            // get total count
            uint32_t elem_size = 0;
            lsf::util::UnSerialize(pbase, total, off, elem_size);

            // record begin time
            timeval tv_begin, tv_end;
            ::gettimeofday(&tv_begin, NULL);

            // recover every elem
            for (int i = 0; i < elem_size && i < _pool.max_size(); ++i)
            {
                // create elem
                uint32_t id = _pool.Malloc();
                ElemType & elem = _pool.Get(id);

                // recover from buffer
                if (!elem.UnSerialize(pbase, total, off)) 
                {
                    LSF_LOG_ERR("unserialize failed, count=%d, %s", i, lsf::util::ProtobufLog::Instance()->ErrCharStr());
                    return false;
                }
            }

            ::gettimeofday(&tv_end, NULL);
            LSF_LOG_INFO("success recover elem, num=%d, utime=%u", elem_size,
                    (tv_end.tv_sec - tv_begin.tv_sec) * 1000000 + (tv_end.tv_usec - tv_begin.tv_usec));

            // delete shm
            lsf::container::SharedMem::Delete(_shm_key); 
        }

        return true;
    }

    ////////////////////////////////////////////////////////////
    // when manager released, serialize all data
    bool Release()
    {
        // if shm already exist, then delete it
        if (lsf::container::SharedMem::IsShmExist(_shm_key) && 
            !lsf::container::SharedMem::Delete(_shm_key))
        {
            LSF_LOG_ERR("delete shared mem failed, %s", lsf::container::SharedMem::SysErrCharStr());
            return false;
        }

        // count need byte size
        size_t byte_size = 0;
        for (auto it : _pool)
        {
            byte_size += it->GetSize();
        }

        // create shm mem
        if (!lsf::container::SharedMem::Create(_shm_key, byte_size))
        {
            LSF_LOG_ERR("create shared mem failed, key=%x, byte_size=%u, max_size=%u, %s",
                    _shm_key, byte_size, _pool.max_size(), lsf::container::SharedMem::SysErrCharStr());
            return false;
        }
        lsf::container::SharedMem shared_mem(_shm_key);
        if (!shared_mem.IsAttached())
        {
            LSF_LOG_ERR("shared mem is not attached, key=%x", _shm_key);
            return false;
        }
        uint8_t * pbase = (uint8_t *)shared_mem.GetPtr();
        size_t total = shared_mem.GetSize();
        size_t off = 0;

        // serialize total count
        lsf::util::Serialize(pbase, total, off, (uint32_t)_pool.size());
        
        // serialize every elem
        for (auto it : _pool)
        {
            if (!it->Serialize(pbase, total, off))
            {
                LSF_LOG_ERR("serialize failed, total=%u, off=%u, %s", total, off, lsf::util::ProtobufLog::Instance()->ErrCharStr());
                return false;
            }
        }

        return true;
    }


public:
    bool IsFull() const { return _pool.IsFull(); } 

    uint32_t size() const { return _pool.size(); }

    uint32_t max_size() const { return _pool.max_size(); }

    iterator begin() { return _pool.being(); }

    iterator end() { return _pool.end(); }

protected:
    pool_type   _pool;
    key_t       _shm_key;
};



// vim:ts=4:sw=4:et:ft=cpp:
