// File:        list.hpp
// Description: ---
// Notes:       ---
// Author:      leoxiang <leoxiang727@qq.com>
// Revision:    2012-05-30 by leoxiang

#pragma once

#include "lsf/container/detail/basic_container.hpp"
#include "lsf/container/detail/impl_bidirectional_list.hpp"
#include "lsf/container/shared_mem.hpp"
#include "lsf/basic/empty_type.hpp"

namespace lsf {
namespace container {

////////////////////////////////////////////////////////////
// List
////////////////////////////////////////////////////////////
template <typename ElemType = lsf::basic::EmptyType, typename StoreType = SharedMem, typename SizeType = size_t>
class List : public detail::BasicContainer<ElemType, SizeType, StoreType, detail::ListState<ElemType, SizeType>,
                                           detail::ListIterator<ElemType, SizeType>> {
public:
    typedef detail::BasicContainer<ElemType, SizeType, StoreType, detail::ListState<ElemType, SizeType>,
                                   detail::ListIterator<ElemType, SizeType>> base_type;
    typedef typename base_type::value_type value_type;
    typedef typename base_type::size_type size_type;
    typedef typename base_type::iterator iterator;
    typedef typename base_type::reverse_iterator reverse_iterator;

public:
    // common list funcs
    bool PushBack(value_type const& val) {
        if (base_type::full()) return false;

        size_type pos = base_type::_ptr_state->GetNewNodeAndInsert(_GetPrevPos(_GetHeadPos()));
        new (_GetDataPtr(pos)) value_type(val);
        return true;
    }

    bool PushFront(value_type const& val) {
        if (base_type::full()) return false;

        size_type pos = base_type::_ptr_state->GetNewNodeAndInsert(_GetHeadPos());
        new (_GetDataPtr(pos)) value_type(val);
        return true;
    }

    bool PopFront() {
        if (base_type::empty()) return false;

        base_type::_ptr_state->FreeNode(_GetHeadPos());
        return true;
    }

    bool PopBack() {
        if (base_type::empty()) return false;

        base_type::_ptr_state->FreeNode(_GetPrevPos(_GetHeadPos()));
        return true;
    }

    bool Insert(iterator iter, value_type const& val) {
        if (base_type::full()) return false;

        size_type pos = base_type::_ptr_state->GetNewNodeAndInsert(iter.GetPos());
        new (_GetDataPtr(pos)) value_type(val);
        return true;
    }

    bool Erase(iterator iter) {
        if (base_type::empty()) return false;

        // call destructor
        iter->~value_type();

        // move to empty list
        base_type::_ptr_state->FreeNode(iter.GetPos());
        return true;
    }

    bool Erase(iterator iter_st, iterator iter_ed) {
        for (iterator iter = iter_st; iter != iter_ed;) {
            if (!Erase(iter++)) return false;
        }
        return true;
    }

    iterator Find(value_type const& val) {
        for (size_type pos = _GetHeadPos(); pos != _GetPrevPos(_GetHeadPos()); pos = _GetNextPos(pos)) {
            if (*_GetDataPtr(pos) == val) return iterator(base_type::_ptr_state, pos);
        }
        return end();
    }

    // accessor
    iterator GetFront() { return iterator(base_type::_ptr_state, _GetHeadPos()); }
    iterator GetBack() { return iterator(base_type::_ptr_state, _GetPrevPos(_GetPrevPos(_GetHeadPos()))); }

    iterator begin() { return iterator(base_type::_ptr_state, _GetHeadPos()); }
    iterator end() { return iterator(base_type::_ptr_state, _GetPrevPos(_GetHeadPos())); }

    reverse_iterator rbegin() { return reverse_iterator(end()); }
    reverse_iterator rend() { return reverse_iterator(begin()); }

private:
    value_type* _GetDataPtr(size_type pos) { return base_type::_ptr_state->GetDataPtr(pos); }
    size_type& _GetNextPos(size_type pos) { return base_type::_ptr_state->GetNextPos(pos); }
    size_type& _GetPrevPos(size_type pos) { return base_type::_ptr_state->GetPrevPos(pos); }
    size_type& _GetHeadPos() { return base_type::_ptr_state->GetHeadPos(); }
    size_type& _GetSize() { return base_type::_ptr_state->GetSize(); }
};

}  // end of namespace container
}  // end of namespace lsf

// vim:ts=4:sw=4:et:ft=cpp:
