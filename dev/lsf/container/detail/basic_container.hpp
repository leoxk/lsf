// File:        basic_container.hpp
// Description: ---
// Notes:       ---
// Author:      leoxiang <leoxiang727@qq.com>
// Revision:    2012-07-31 by leoxiang

#pragma once

#include <stdint.h>
#include <iterator>
#include "lsf/basic/macro.hpp"
#include "lsf/basic/noncopyable.hpp"
#include "lsf/basic/error.hpp"
#include "lsf/basic/empty_type.hpp"

namespace lsf {
namespace container {
namespace detail {

// for container which do not need iterator
class EmptyIterator {};

////////////////////////////////////////////////////////////
// BasicBasicContainer
////////////////////////////////////////////////////////////
template <typename ElemType, typename SizeType, typename StoreType, typename StateType,
          typename IteratorType = EmptyIterator>
class BasicContainer : public basic::Error, public basic::NonCopyable {
public:
    typedef ElemType value_type;
    typedef SizeType size_type;
    typedef IteratorType iterator;
    typedef StateType state_type;
    typedef std::reverse_iterator<iterator> reverse_iterator;

public:
    // bind and recover storage
    bool BindAndRecoverStorage(StoreType store) {
        _ptr_state = nullptr;
        _store = store;

        if (_store.GetPtr() == nullptr) {
            ErrString() = LSF_DEBUG_INFO + _store.ErrString();
            return false;
        }

        if (((StateType *)_store.GetPtr())->ElemByteSize() != sizeof(value_type)) {
            ErrString() = LSF_DEBUG_INFO;
            return false;
        }

        _ptr_state = (StateType *)_store.GetPtr();
        return true;
    }

    // bind and initiate storage
    bool BindAndInitStorage(StoreType store) {
        _ptr_state = nullptr;
        _store = store;

        if (_store.GetPtr() == nullptr) {
            ErrString() = LSF_DEBUG_INFO + _store.ErrString();
            return false;
        }

        _ptr_state = (StateType *)_store.GetPtr();
        _ptr_state->Init(_store.GetSize());
        return true;
    }

    // accessor funcs
    size_type size() const { return _ptr_state->size(); }
    size_type max_size() const { return _ptr_state->max_size(); }
    size_t ElemByteSize() const { return _ptr_state->ElemByteSize(); }

    bool full() const { return _ptr_state->full(); }
    bool empty() const { return _ptr_state->empty(); }
    bool IsBindStorage() const { return _ptr_state != nullptr; }

    // static funcs
    static size_t CalcByteSize(size_type size) { return StateType::CalcByteSize(size); }
    static size_t CalcElemByteSize(StoreType store) { return StateType::CalcElemByteSize(store.GetPtr()); }
    static size_t CalcElemMaxSize(StoreType store) { return StateType::CalcElemMaxSize(store.GetPtr()); }

protected:
    StateType *_ptr_state = nullptr;
    StoreType _store;
};

}  // end of namespace detail
}  // end of namespace container
}  // end of namespace lsf

// vim:ts=4:sw=4:et:ft=cpp:
