// File:        obj_pool.hpp
// Description: ---
// Notes:       ---
// Author:      leoxiang <leoxiang727@qq.com>
// Revision:    2012-07-31 by leoxiang

#pragma once

#include <stdexcept>
#include "lsf/basic/macro.hpp"
#include "lsf/container/detail/basic_container.hpp"
#include "lsf/container/detail/impl_bidirectional_list.hpp"
#include "lsf/container/shared_mem.hpp"

namespace lsf {
namespace container {
    
////////////////////////////////////////////////////////////
// Pool
////////////////////////////////////////////////////////////
template<
    typename ElemType,
    typename StoreType = SharedMem,
    typename SizeType = size_t>
class Pool :
    public detail::BasicContainer<
        ElemType,
        SizeType,
        StoreType,
        detail::ListState<ElemType, SizeType>,
        detail::ListIterator<ElemType, SizeType> >
{
public:
    typedef detail::BasicContainer<
        ElemType, 
        SizeType, 
        StoreType,
        detail::ListState<ElemType, SizeType>,
        detail::ListIterator<ElemType, SizeType> >  base_type;
    typedef typename base_type::value_type          value_type;
    typedef typename base_type::size_type           size_type;
    typedef typename base_type::iterator            iterator;
    typedef typename base_type::reverse_iterator    reverse_iterator;

public:
    // common list funcs
    size_type Malloc() 
    {
        if (base_type::full()) throw std::runtime_error(LSF_DEBUG_INFO);

        size_type pos = base_type::_ptr_state->GetNewNodeAndInsert(_GetPrevPos(_GetHeadPos()));
        new(_GetDataPtr(pos)) value_type();
        return pos;
    }

    size_type Malloc(value_type const & val) 
    {
        if (base_type::full()) throw std::runtime_error(LSF_DEBUG_INFO);

        size_type pos = base_type::_ptr_state->GetNewNodeAndInsert(_GetPrevPos(_GetHeadPos()));
        new(_GetDataPtr(pos)) value_type(val);
        return pos;
    }

    bool Free(iterator iter) { return Free(iter.GetPos()); }

    bool Free(size_type index) 
    {
        if (_GetDataPtr(index) == NULL) throw std::runtime_error(LSF_DEBUG_INFO);

        // call destructor
        _GetDataPtr(index)->~value_type();

        // move to empty list
        base_type::_ptr_state->FreeNode(index);
        return true;
    }

    value_type * GetPtr(size_type index) { return _GetDataPtr(index); }

    value_type & Get(size_type index) 
    {
        value_type * ptr = _GetDataPtr(index);
        if (ptr == NULL) throw std::runtime_error(LSF_DEBUG_INFO);
        return *ptr;
    }

    // accessor
    iterator begin() { return iterator(base_type::_ptr_state, _GetHeadPos()); }
    iterator end()   { return iterator(base_type::_ptr_state, _GetPrevPos(_GetHeadPos())); }

    reverse_iterator rbegin() { return reverse_iterator(end()); }
    reverse_iterator rend()   { return reverse_iterator(begin()); }

private:
    value_type * _GetDataPtr(size_type pos) const { return base_type::_ptr_state->GetDataPtr(pos); }
    size_type    _GetNextPos(size_type pos) const { return base_type::_ptr_state->GetNextPos(pos); }
    size_type    _GetPrevPos(size_type pos) const { return base_type::_ptr_state->GetPrevPos(pos); }
    size_type    _GetHeadPos()              const { return base_type::_ptr_state->GetHeadPos(); }
};

} // end of namespace container
} // end of namespace lsf

// vim:ts=4:sw=4:et:ft=cpp:
