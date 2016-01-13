// File:        set.hpp
// Description: ---
// Notes:       ---
// Author:      leoxiang <leoxiang727@qq.com>
// Revision:    2012-05-30 by leoxiang

#pragma once

#include "lsf/container/detail/basic_container.hpp"
#include "lsf/container/detail/impl_red_black_tree.hpp"
#include "lsf/container/shared_mem.hpp"
#include "lsf/basic/type_cast.hpp"

namespace lsf {
namespace container {

////////////////////////////////////////////////////////////
// Set
////////////////////////////////////////////////////////////
template <typename ElemType, typename StoreType = SharedMem, typename SizeType = size_t>
class Set : public detail::BasicContainer<ElemType, SizeType, StoreType, detail::RBTreeState<ElemType, SizeType>,
                                          detail::RBTreeIterator<ElemType, SizeType>> {
public:
    using base_type = detail::BasicContainer<ElemType, SizeType, StoreType, detail::RBTreeState<ElemType, SizeType>,
                                   detail::RBTreeIterator<ElemType, SizeType>>;
    using value_type = typename base_type::value_type;
    using size_type = typename base_type::size_type;
    using state_type = typename base_type::state_type;
    using iterator = typename base_type::iterator;
    using reverse_iterator = typename base_type::reverse_iterator;
    using const_iterator = iterator const;
    using const_reverse_iterator = reverse_iterator const;

public:
    bool Insert(value_type const& val) {
        if (base_type::full()) return false;

        // get insert pos
        size_type pos = base_type::_ptr_state->GetNewNodeAndInsert(val);
        if (pos == state_type::NPOS) return false;

        // construct value type
        new (base_type::_ptr_state->GetDataPtr(pos)) value_type(val);
        return true;
    }

    bool Erase(value_type const& val) {
        if (base_type::empty()) return false;

        // find pos
        std::pair<size_type, size_type> pos_pair = base_type::_ptr_state->FindNode(val);
        if (pos_pair.first == state_type::NPOS) return false;

        // erase pos
        base_type::_ptr_state->EraseNode(pos_pair);

        // call destructor
        base_type::_ptr_state->GetDataPtr(pos_pair.first)->~value_type();
        return true;
    }

    iterator Find(value_type const& val) {
        size_type pos = base_type::_ptr_state->FindNode(val).first;
        // not found
        if (pos == state_type::NPOS) return end();

        return iterator(base_type::_ptr_state, pos);
    }

    // accessor
    iterator operator[](value_type const& val) { return Find(val); }

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
