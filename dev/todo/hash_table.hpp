// File:        hash_table.hpp
// Description: ---
// Notes:       ---
// Author:      leoxiang <leoxiang727@qq.com>
// Revision:    2012-05-07 by leoxiang

#pragma once

#include "lsf/storage/shared_mem.hpp"

#include <string>

namespace lsf {
namespace container {

template<typename K, typename V>
class HashTable
{

public:
    // typedefs

public:
    //////////////////////////////////////////////////////////// 
    // constructor
    explicit HashTable() { }

    explicit HashTable(SharedMem const & shm) { 
        Bind(shm);
    }
    
    //////////////////////////////////////////////////////////// 
    // Bind and UnBind
    bool Bind(SharedMem const & shm) {
        if (!shm.IsAttached()) return false;
        _shm = shm;
        return true;
    }

    bool UnBind() {
        if (_shm.IsAttached()) _shm.Detach();
        return true;
    }

    T & Get() { }

    //////////////////////////////////////////////////////////// 
    // other funcs
    bool IsBind() const { return _shm.IsAttached(); }

    operator bool() const { return IsBind(); }

private:
    SharedMem   _shm;
    size_t      _length;
    size_t      _depth;
    string      _err_string;
}


template<typename K, typename V>
class HashTableIntrusive
{

};

} // end of namespace container
} // end of namespace lsf


// vim:ts=4:sw=4:et:ft=cpp:
