// File:        impl_static_array.hpp
// Description: ---
// Notes:       ---
// Author:      leoxiang <leoxiang727@qq.com>
// Revision:    2012-10-20 by leoxiang

#pragma once

#include <cstddef>
#include <iterator>

namespace lsf {
namespace container {
namespace detail {

////////////////////////////////////////////////////////////
// StaticArrayState
////////////////////////////////////////////////////////////
template <typename ElemType, typename SizeType>
class StaticArrayState {
public:
    using value_type = ElemType;
    using size_type = SizeType;
    using this_type = StaticArrayState;

public:
    void Init(size_t byte_size) {
        _max_size = (byte_size - sizeof(this_type)) / sizeof(value_type);
        _elem_byte_size = sizeof(value_type);
        // make sure all obj is constructed
        for (size_t index = 0; index < _max_size; index++) {
            new (GetDataPtr(index)) value_type();
        }
    }

    static size_t CalcByteSize(size_type size) { return size * sizeof(value_type) + sizeof(this_type); }
    static size_t CalcElemByteSize(void const *ptr) { return ((this_type const *)ptr)->ElemByteSize(); }
    static size_t CalcElemMaxSize(void const *ptr) { return ((this_type const *)ptr)->max_size(); }

    size_type size() const { return _max_size; }
    size_type max_size() const { return _max_size; }
    size_t ElemByteSize() const { return _elem_byte_size; }

    bool empty() const { return true; }
    bool full() const { return true; }

    value_type *GetDataPtr(size_type pos) const { return (value_type *)(this + 1) + pos; }

private:
    size_type _max_size;
    size_type _elem_byte_size;
};

////////////////////////////////////////////////////////////
// StaticArrayIterator
////////////////////////////////////////////////////////////
// forward
template <typename ElemType, typename SizeType>
class StaticArrayIterator : public std::iterator<std::bidirectional_iterator_tag, ElemType> {
public:
    using base_type = std::iterator<std::bidirectional_iterator_tag, ElemType>;
    using value_type = typename base_type::value_type;
    using pointer = typename base_type::pointer;
    using reference = typename base_type::reference;
    using difference_type = typename base_type::difference_type;
    using state_type = StaticArrayState<ElemType, SizeType>;
    using size_type = SizeType;

public:
    // constructor
    StaticArrayIterator(state_type *ptr_state = nullptr, size_type pos = 0) : _ptr_state(ptr_state), _pos(pos) {}

    // member funcs
    StaticArrayIterator &operator++() {
        _pos++;
        return *this;
    }
    StaticArrayIterator &operator--() {
        _pos--;
        return *this;
    }

    pointer operator->() const { return _ptr_state->GetDataPtr(_pos); }
    reference operator*() const { return *(_ptr_state->GetDataPtr(_pos)); }

    StaticArrayIterator operator++(int) {
        StaticArrayIterator tmp = *this;
        ++*this;
        return tmp;
    }
    StaticArrayIterator operator--(int) {
        StaticArrayIterator tmp = *this;
        --*this;
        return tmp;
    }

    bool operator==(StaticArrayIterator const &rhs) { return _ptr_state == rhs._ptr_state && _pos == rhs._pos; }
    bool operator!=(StaticArrayIterator const &rhs) { return !(_ptr_state == rhs._ptr_state && _pos == rhs._pos); }

private:
    state_type *_ptr_state;
    size_type _pos;
};

}  // end of namespace detail
}  // end of namespace container
}  // end of namespace lsf

// vim:ts=4:sw=4:et:ft=cpp:
