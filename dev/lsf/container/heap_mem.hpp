// File:        basic_storage.hpp
// Description: ---
// Notes:       ---
// Author:      leoxiang <leoxiang727@qq.com>
// Revision:    2012-08-01 by leoxiang

#pragma once

#include "lsf/basic/error.hpp"

namespace lsf {
namespace container {

class HeapMem : public basic::Error {
public:
    // contructor and destructor
    HeapMem() { }
    HeapMem(size_t byte_size) { Malloc(byte_size); }

    HeapMem(HeapMem const &rhs) {
        _ptr_mem = rhs._ptr_mem;
        _ptr_use_cnt = rhs._ptr_use_cnt;
        _byte_size = rhs._byte_size;
        if (_ptr_use_cnt != nullptr) *_ptr_use_cnt += 1;
    }

    ~HeapMem() { _Release(); }

    // functions
    void Malloc(size_t byte_size) {
        _Release();

        _ptr_mem = malloc(byte_size);
        _ptr_use_cnt = (size_t *)malloc(sizeof(size_t));
        _byte_size = byte_size;
        if (_ptr_use_cnt != nullptr) *_ptr_use_cnt = 1;
    }

    HeapMem &operator=(HeapMem const &rhs) {
        if (this == &rhs) return *this;

        _Release();
        _ptr_mem = rhs._ptr_mem;
        _ptr_use_cnt = rhs._ptr_use_cnt;
        _byte_size = rhs._byte_size;
        if (_ptr_use_cnt != nullptr) *_ptr_use_cnt += 1;
        return *this;
    }

    void *GetPtr() const { return _ptr_mem; }
    size_t GetSize() const { return _byte_size; }
    size_t GetUseCount() const { return *_ptr_use_cnt; }

private:
    void _Release() {
        if (_ptr_use_cnt == nullptr) return;

        *_ptr_use_cnt -= 1;
        if (*_ptr_use_cnt <= 0) {
            free(_ptr_mem);
            free(_ptr_use_cnt);
        }
    }

    void *_ptr_mem = nullptr;
    size_t *_ptr_use_cnt = nullptr;
    size_t _byte_size = 0;
};

}  // end of namespace container
}  // end of namespace lsf

// vim:ts=4:sw=4:et:ft=cpp:
