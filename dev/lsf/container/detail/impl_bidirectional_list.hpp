// File:        impl_bidirectional_list.hpp
// Description: ---
// Notes:       ---
// Author:      leoxiang <leoxiang727@qq.com>
// Revision:    2012-10-20 by leoxiang

#pragma once

namespace lsf {
namespace container {
namespace detail {

////////////////////////////////////////////////////////////
// ListNode
////////////////////////////////////////////////////////////
template<
    typename ElemType,
    typename SizeType>
struct ListNode
{
    SizeType   prev_pos;
    SizeType   next_pos;
    ElemType   data;
    bool       is_use;
};

////////////////////////////////////////////////////////////
// ListState
////////////////////////////////////////////////////////////
template<
    typename ElemType,
    typename SizeType>
class ListState
{
public:
    typedef ElemType                       value_type;
    typedef SizeType                       size_type;  
    typedef ListNode<ElemType, SizeType>   node_type;
    typedef ListState<ElemType, SizeType>  this_type;

    static const size_type ENDPOS = 0;

public:
    void Init(size_t byte_size) {
        _size           = 0;
        _max_size       = (byte_size - sizeof(this_type)) / sizeof(node_type);
        _elem_byte_size = sizeof(value_type);
        _empty_head_pos = 1;
        _head_pos       = 0;

        // make list a loop
        GetIsUse(ENDPOS)   = false;
        GetNextPos(ENDPOS) = ENDPOS;
        GetPrevPos(ENDPOS) = ENDPOS;

        // make empty list a loop
        for (size_type pos = 1; pos < _max_size; pos++) {
            GetIsUse(pos)   = false;
            GetNextPos(pos) = pos + 1;
            GetPrevPos(pos) = pos - 1;
        }
        GetPrevPos(1) = _max_size - 1;
        GetNextPos(_max_size - 1) = 1;
    }

    // get a node from empty list, and insert into list
    size_type GetNewNodeAndInsert(size_type insert_pos)
    {
        // get node from empty list
        size_type pos = _empty_head_pos;
        if (GetNextPos(_empty_head_pos) == _empty_head_pos) { // single node loop
            _empty_head_pos = ENDPOS;
        }
        else {
            GetPrevPos(GetNextPos(_empty_head_pos)) = GetPrevPos(_empty_head_pos);
            GetNextPos(GetPrevPos(_empty_head_pos)) = GetNextPos(_empty_head_pos);
            _empty_head_pos = GetNextPos(_empty_head_pos);
        }

        // insert pos before the insert_pos in list
        _size++;
        GetIsUse(pos) = true;
        GetNextPos(pos) = insert_pos;
        GetPrevPos(pos) = GetPrevPos(insert_pos);
        GetPrevPos(GetNextPos(pos)) = pos;
        GetNextPos(GetPrevPos(pos)) = pos;
        if (_head_pos == insert_pos || _head_pos == ENDPOS) {
            _head_pos = pos;
        }

        return pos;
    }

    // free node from the list and insert into empty list
    void FreeNode(size_type pos)
    {
        // free node from list
        GetPrevPos(GetNextPos(pos)) = GetPrevPos(pos);
        GetNextPos(GetPrevPos(pos)) = GetNextPos(pos);
        GetIsUse(pos) = false;
        if (_head_pos == pos) {
            _head_pos = GetNextPos(pos);
        }

        // insert node into empty list's tail
        _size--;
        GetIsUse(pos) = false;
        if ( _empty_head_pos == ENDPOS) { // none node, so form a sinlge loop node
            GetNextPos(pos) = pos;
            GetPrevPos(pos) = pos;
            _empty_head_pos = pos;
        }
        else {
            GetNextPos(pos) = _empty_head_pos;
            GetPrevPos(pos) = GetPrevPos(_empty_head_pos);
            GetPrevPos(GetNextPos(pos)) = pos;
            GetNextPos(GetPrevPos(pos)) = pos;
        }
    }

    static size_t CalcByteSize(size_type size) { return (size + 1) * sizeof(node_type) + sizeof(this_type); }
    static size_t CalcElemByteSize(void const * ptr) { return ((this_type const *)ptr)->ElemByteSize(); }
    static size_t CalcElemMaxSize(void const * ptr) { return ((this_type const *)ptr)->MaxSize(); }

    size_type Size()    const { return _size; }
    size_type MaxSize() const { return _max_size - 1; }
    size_t ElemByteSize() const { return _elem_byte_size; }

    bool IsFull()  const { return _size == _max_size - 1; }
    bool IsEmpty() const { return _size == 0; }

    node_type  * GetNodePtr(size_type pos) const { return (node_type *)(this + 1) + pos; }

    value_type * GetDataPtr(size_type pos) const 
    { 
        if (pos >= _max_size) return NULL;
        node_type * ptr_node = GetNodePtr(pos);

        if (!ptr_node->is_use)  return NULL;
        return &(ptr_node->data);
    }

    size_type & GetHeadPos()              { return _head_pos; }
    size_type & GetNextPos(size_type pos) { return GetNodePtr(pos)->next_pos; }
    size_type & GetPrevPos(size_type pos) { return GetNodePtr(pos)->prev_pos; }
    bool      & GetIsUse(size_type pos)   { return GetNodePtr(pos)->is_use; }

private:
    size_type      _size;
    size_type      _max_size; // here use first node to mark END
    size_type      _elem_byte_size;
    size_type      _empty_head_pos;
    size_type      _head_pos;
};

////////////////////////////////////////////////////////////
// ListIterator
////////////////////////////////////////////////////////////
template<
    typename ElemType,
    typename SizeType>
class ListIterator
{
public:
    typedef std::bidirectional_iterator_tag     iterator_category;
    typedef ElemType                            value_type;
    typedef ElemType *                          pointer;
    typedef ElemType &                          reference;
    typedef int                                 difference_type;
    typedef ListState<ElemType, SizeType>       state_type;
    typedef SizeType                            size_type;  

public:
    // constructor
    ListIterator(state_type * ptr_state = NULL, size_type pos = 0)
        : _ptr_state(ptr_state), _pos(pos)
    { }

    ListIterator(ListIterator const & rhs)
        : _ptr_state(rhs._ptr_state), _pos(rhs._pos)
    { }

    ListIterator & operator=(ListIterator const & rhs) {
        if (&rhs == this) return *this;

        _ptr_state = rhs._ptr_state;
        _pos       = rhs._pos;
        return *this;
    }

    // member funcs
    ListIterator operator+(difference_type diff) const {
        ListIterator tmp = *this;

        if (diff < 0) return tmp - (-diff);
        while (diff != 0) {
            ++tmp;
            --diff;
        }
        return tmp;
    }

    ListIterator operator-(difference_type diff) const {
        ListIterator tmp = *this;

        if (diff < 0) return tmp + (-diff);
        while (diff != 0) {
            --tmp;
            --diff;
        }
        return tmp;
    }

    ListIterator & operator++() { _pos = _ptr_state->GetNextPos(_pos); return *this; }
    ListIterator & operator--() { _pos = _ptr_state->GetPrevPos(_pos); return *this; } 

    pointer  operator->() { return _ptr_state->GetDataPtr(_pos); }
    reference operator*() { return *(_ptr_state->GetDataPtr(_pos)); }

    ListIterator operator++(int) { ListIterator tmp = *this; ++*this; return tmp; } 
    ListIterator operator--(int) { ListIterator tmp = *this; --*this; return tmp; } 
    
    bool operator==(ListIterator const & rhs) const { return _ptr_state == rhs._ptr_state && _pos == rhs._pos; }
    bool operator!=(ListIterator const & rhs) const { return !(_ptr_state == rhs._ptr_state && _pos == rhs._pos); }

    size_type GetPos() const { return _pos; }

private:
    state_type*     _ptr_state;
    size_type       _pos;
};

} // end of namespace detail
} // end of namespace container
} // end of namespace lsf

// vim:ts=4:sw=4:et:ft=cpp:
