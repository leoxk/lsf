// File:        set.hpp
// Description: ---
// Notes:       ---
// Author:      leoxiang <leoxiang@tencent.com>
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
template<
    typename ElemType,
    typename StoreType = SharedMem,
    typename SizeType = size_t>
class Set : 
    public detail::BasicContainer<
        ElemType,
        SizeType,
        StoreType,
        detail::RBTreeState<ElemType, SizeType>,
        detail::RBTreeIterator<ElemType, SizeType> >
{
public:
    typedef detail::BasicContainer<
        ElemType, 
        SizeType, 
        StoreType,
        detail::RBTreeState<ElemType, SizeType>, 
        detail::RBTreeIterator<ElemType, SizeType> >    base_type;
    typedef typename base_type::value_type              value_type;
    typedef typename base_type::size_type               size_type;
    typedef typename base_type::state_type              state_type;
    typedef typename base_type::iterator                iterator;
    typedef typename base_type::reverse_iterator        reverse_iterator;
    typedef iterator const                              const_iterator;
    typedef reverse_iterator const                      const_reverse_iterator;

public:
    bool Insert(value_type const & val) 
    {
        if (base_type::IsFull())    return false;

        // get insert pos
        size_type pos = base_type::_ptr_state->GetNewNodeAndInsert(val);
        if (pos == state_type::NPOS) return false;

        // construct value type
        new(base_type::_ptr_state->GetDataPtr(pos)) value_type(val);
        return true;
    }

    bool Erase(value_type const & val)
    {
        if (base_type::IsEmpty())   return false;

        // find pos
        std::pair<size_type, size_type> pos_pair = base_type::_ptr_state->FindNode(val);
        if (pos_pair.first == state_type::NPOS) return false;

        // erase pos
        base_type::_ptr_state->EraseNode(pos_pair);

        // call destructor
        base_type::_ptr_state->GetDataPtr(pos_pair.first)->~value_type();
        return true;
    }
    
    iterator Find(value_type const & val) 
    {
        size_type pos = base_type::_ptr_state->FindNode(val).first;
        // not found
        if (pos == state_type::NPOS) return End();

        return iterator(base_type::_ptr_state, pos);
    }

    // accessor
    iterator operator[](value_type const & val) { return Find(val); }

    iterator Begin() { return iterator(base_type::_ptr_state, base_type::_ptr_state->FindSmallestPos()); }
    iterator End()   { return iterator(base_type::_ptr_state, base_type::_ptr_state->NPOS); }
    const_iterator Begin() const { return iterator(base_type::_ptr_state, base_type::_ptr_state->FindSmallestPos()); }
    const_iterator End()   const { return iterator(base_type::_ptr_state, base_type::_ptr_state->NPOS); }

    reverse_iterator RBegin() { return reverse_iterator(End()); }
    reverse_iterator REnd()   { return reverse_iterator(Begin()); }
    const_reverse_iterator RBegin() const { return reverse_iterator(End()); }
    const_reverse_iterator REnd()   const { return reverse_iterator(Begin()); }

    ////////////////////////////////////////////////////////////
    // for debug
    bool CheckConsist() const {
        return base_type::_ptr_state->CheckConsist();
    }

    iterator FindRoot() 
    {
        if (!base_type::IsBindStorage()) return End();

        size_type root_pos = base_type::_ptr_state->GetRootPos();
        if (root_pos == state_type::NPOS) return End();

        return iterator(base_type::_ptr_state, root_pos);
    }

    std::string ToString(size_type pos) 
    {
        size_type lchild_pos = base_type::_ptr_state->GetLChildPos(pos);
        size_type rchild_pos = base_type::_ptr_state->GetRChildPos(pos);
        size_type parent_pos = base_type::_ptr_state->GetParentPos(pos);

        return _ToString(pos) + ": " + _ToString(lchild_pos) + 
            ", " + _ToString(rchild_pos) + ", " + _ToString(parent_pos);
    }

    std::string _ToString(size_type pos)
    {
        std::string out;

        out = "[" + lsf::basic::TypeCast<std::string>(pos) + "]";
        if (base_type::_ptr_state->GetColor(pos) == state_type::RED) {
            out += "[*]";
        }
        else {
            out += "[ ]";
        }
        return out;
    }
};

} // end of namespace container
} // end of namespace lsf

// vim:ts=4:sw=4:et:ft=cpp:
