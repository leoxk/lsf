// File:        impl_circular_queue.hpp
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
// QueueState
////////////////////////////////////////////////////////////
template <typename ElemType, typename SizeType>
class QueueState {
public:
    using value_type = ElemType;
    using size_type = SizeType;
    using this_type = QueueState;

public:
    void Init(size_t byte_size) {
        _size = 0;
        _max_size = (byte_size - sizeof(this_type)) / sizeof(value_type);
        _elem_byte_size = sizeof(value_type);
        _head_pos = 0;
        _tail_pos = 0;
    }

    static size_t CalcByteSize(size_type size) { return (size + 1) * sizeof(value_type) + sizeof(this_type); }
    static size_t CalcElemByteSize(void const *ptr) { return ((this_type const *)ptr)->ElemByteSize(); }
    static size_t CalcElemMaxSize(void const *ptr) { return ((this_type const *)ptr)->max_size(); }

    size_type size() const { return (_tail_pos - _head_pos + _max_size) % _max_size; }
    size_type max_size() const { return _max_size - 1; }
    size_t ElemByteSize() const { return _elem_byte_size; }

    bool empty() const { return _head_pos == _tail_pos; }
    bool full() const { return _head_pos == (_tail_pos + 1) % _max_size; }

    value_type *GetDataPtr(size_type pos) const { return (value_type *)(this + 1) + pos % _max_size; }

    size_type &GetHeadPos() { return _head_pos; }
    size_type &GetTailPos() { return _tail_pos; }

    size_type CalcAbsolutePos(size_type pos) const { return (pos + _max_size) % _max_size; }

    void HeadPosInc() { _head_pos = (_head_pos + 1) % _max_size; }
    void HeadPosDec() { _head_pos = (_head_pos - 1 + _max_size) % _max_size; }
    void TailPosInc() { _tail_pos = (_tail_pos + 1) % _max_size; }
    void TailPosDec() { _tail_pos = (_tail_pos - 1 + _max_size) % _max_size; }

private:
    size_type _size;
    size_type _max_size;  // notice: here use one node to mark end of queue
    size_type _elem_byte_size;
    size_type _head_pos;
    size_type _tail_pos;
};

////////////////////////////////////////////////////////////
// QueueIterator
////////////////////////////////////////////////////////////
// forward
template <typename ElemType, typename SizeType>
class QueueIterator : public std::iterator<std::bidirectional_iterator_tag, ElemType>{
public:
    using base_type = std::iterator<std::bidirectional_iterator_tag, ElemType>;
    using value_type = typename base_type::value_type;
    using pointer = typename base_type::pointer;
    using reference = typename base_type::reference;
    using difference_type = typename base_type::difference_type;
    using state_type = QueueState<ElemType, SizeType>;
    using size_type = SizeType;

public:
    // constructor
    QueueIterator(state_type *ptr_state = nullptr, size_type idx = 0) : _ptr_state(ptr_state), _pos(0) {
        // convert relative idx to abs pos
        if (_ptr_state != nullptr) {
            _pos = _ptr_state->CalcAbsolutePos(_ptr_state->GetHeadPos() + idx);
        }
    }

    // member funcs
    QueueIterator &operator++() {
        _pos = _ptr_state->CalcAbsolutePos(_pos + 1);
        return *this;
    }

    QueueIterator &operator--() {
        _pos = _ptr_state->CalcAbsolutePos(_pos - 1);
        return *this;
    }

    pointer operator->() const { return _ptr_state->GetDataPtr(_pos); }
    reference operator*() const { return *(_ptr_state->GetDataPtr(_pos)); }

    QueueIterator operator++(int) {
        QueueIterator tmp = *this;
        ++*this;
        return tmp;
    }
    QueueIterator operator--(int) {
        QueueIterator tmp = *this;
        --*this;
        return tmp;
    }

    bool operator==(QueueIterator const &rhs) { return _ptr_state == rhs._ptr_state && _pos == rhs._pos; }
    bool operator!=(QueueIterator const &rhs) { return !(_ptr_state == rhs._ptr_state && _pos == rhs._pos); }

private:
    state_type *_ptr_state;
    size_type _pos;
};

}  // end of namespace detail
}  // end of namespace container
}  // end of namespace lsf

// vim:ts=4:sw=4:et:ft=cpp:
