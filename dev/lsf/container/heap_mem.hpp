// File:        basic_storage.hpp
// Description: ---
// Notes:       ---
// Author:      leoxiang <leoxiang727@qq.com>
// Revision:    2012-08-01 by leoxiang

#pragma once

#include <memory>
#include "lsf/basic/error.hpp"

namespace lsf {
namespace container {

class HeapMem : public lsf::basic::Error {
public:
    // contructor and destructor
    HeapMem() { }
    HeapMem(size_t byte_size) { Malloc(byte_size); }

    // functions
    void Malloc(size_t byte_size) {
        _byte_size = byte_size;
        _sp.reset(new char[byte_size], std::default_delete<char[]>());
    }

    void *GetPtr() const { return &*_sp; }
    size_t GetSize() const { return _byte_size; }
    size_t GetUseCount() const { return _sp.use_count(); }

private:
    std::shared_ptr<char> _sp;
    size_t _byte_size = 0;
};

}  // end of namespace container
}  // end of namespace lsf

// vim:ts=4:sw=4:et:ft=cpp:
