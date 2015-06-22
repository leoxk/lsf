// File:        map.hpp
// Description: ---
// Notes:       ---
// Author:      leoxiang <leoxiang727@qq.com>
// Revision:    2012-11-28 by leoxiang

#pragma once

#include <stdexcept>
#include "lsf/basic/macro.hpp"
#include "lsf/container/detail/basic_container.hpp"
#include "lsf/container/detail/impl_red_black_tree.hpp"
#include "lsf/container/shared_mem.hpp"

namespace lsf {
namespace container {

////////////////////////////////////////////////////////////
// MapData
////////////////////////////////////////////////////////////
template <typename KeyType, typename MapType>
class MapData {
public:
    MapData(KeyType const& _key) : key(_key) {}
    MapData(KeyType const& _key, MapType const& _value) : key(_key), value(_value) {}

    bool operator!=(MapData const& rhs) const { return key != rhs.key; }
    bool operator==(MapData const& rhs) const { return key == rhs.key; }
    bool operator<(MapData const& rhs) const { return key < rhs.key; }
    bool operator>(MapData const& rhs) const { return key > rhs.key; }

    KeyType key;
    MapType value;
};

////////////////////////////////////////////////////////////
// Map
////////////////////////////////////////////////////////////
template <typename KeyType, typename MapType, typename StoreType = SharedMem, typename SizeType = size_t>
class Map : public detail::BasicContainer<MapData<KeyType, MapType>, SizeType, StoreType,
                                          detail::RBTreeState<MapData<KeyType, MapType>, SizeType>,
                                          detail::RBTreeIterator<MapData<KeyType, MapType>, SizeType>> {
public:
    typedef detail::BasicContainer<MapData<KeyType, MapType>, SizeType, StoreType,
                                   detail::RBTreeState<MapData<KeyType, MapType>, SizeType>,
                                   detail::RBTreeIterator<MapData<KeyType, MapType>, SizeType>> base_type;
    typedef KeyType key_type;
    typedef MapType map_type;
    typedef typename base_type::value_type value_type;
    typedef typename base_type::size_type size_type;
    typedef typename base_type::state_type state_type;
    typedef typename base_type::iterator iterator;
    typedef typename base_type::reverse_iterator reverse_iterator;
    typedef iterator const const_iterator;
    typedef reverse_iterator const const_reverse_iterator;

public:
    bool Insert(key_type const& key, map_type const& val) {
        if (base_type::full()) return false;

        // get insert pos
        size_type pos = base_type::_ptr_state->GetNewNodeAndInsert(value_type(key));
        if (pos == state_type::NPOS) return false;

        // construct value type
        new (base_type::_ptr_state->GetDataPtr(pos)) value_type(key, val);

        return true;
    }

    bool Erase(key_type const& key) {
        if (base_type::empty()) return false;

        // find pos
        std::pair<size_type, size_type> pos_pair = base_type::_ptr_state->FindNode(value_type(key));
        if (pos_pair.first == state_type::NPOS) return false;

        // erase pos
        base_type::_ptr_state->EraseNode(pos_pair);

        // call destructor
        base_type::_ptr_state->GetDataPtr(pos_pair.first)->~value_type();
        return true;
    }

    iterator Find(key_type const& key) {
        size_type pos = base_type::_ptr_state->FindNode(value_type(key)).first;
        // not found
        if (pos == state_type::NPOS) return end();

        return iterator(base_type::_ptr_state, pos);
    }

    // if there is no node, just insert one
    map_type& operator[](key_type const& key) {
        iterator iter = Find(key);
        if (iter == end()) {
            if (!Insert(key, map_type())) throw std::runtime_error(LSF_DEBUG_INFO);
            iter = Find(key);
        }
        return iter->value;
    }

    // accessor
    iterator begin() { return iterator(base_type::_ptr_state, base_type::_ptr_state->FindSmallestPos()); }
    iterator end() { return iterator(base_type::_ptr_state, base_type::_ptr_state->NPOS); }
    const_iterator begin() const { return iterator(base_type::_ptr_state, base_type::_ptr_state->FindSmallestPos()); }
    const_iterator end() const { return iterator(base_type::_ptr_state, base_type::_ptr_state->NPOS); }

    reverse_iterator rbegin() { return reverse_iterator(end()); }
    reverse_iterator rend() { return reverse_iterator(begin()); }
    const_reverse_iterator rbegin() const { return reverse_iterator(end()); }
    const_reverse_iterator rend() const { return reverse_iterator(begin()); }
};

}  // end of namespace container
}  // end of namespace lsf

// vim:ts=4:sw=4:et:ft=cpp:
