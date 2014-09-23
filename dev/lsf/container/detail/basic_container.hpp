// File:        basic_container.hpp
// Description: ---
// Notes:       ---
// Author:      leoxiang <leoxiang@tencent.com>
// Revision:    2012-07-31 by leoxiang

#pragma once

#include <stdint.h>
#include <iterator>
#include "lsf/basic/noncopyable.hpp"
#include "lsf/basic/error.hpp"

namespace lsf {
namespace container {
namespace detail {

// for container which do not need iterator
class EmptyIterator { };

////////////////////////////////////////////////////////////
// BasicBasicContainer
////////////////////////////////////////////////////////////
template<
    typename ElemType,
    typename SizeType,
    typename StoreType,
    typename StateType,
    typename IteratorType = EmptyIterator>
class BasicContainer :
    public basic::Error,
    public basic::NonCopyable
{
public:
    typedef ElemType                           value_type;
    typedef SizeType                           size_type;
    typedef IteratorType                       iterator;
    typedef StateType                          state_type;
    typedef std::reverse_iterator<iterator>    reverse_iterator;

public:
    BasicContainer() : _ptr_state(NULL) { }

    // bind and recover from storage
    bool BindStorage(StoreType store) {
        _ptr_state = NULL;
        _store = store;

        if (_store.GetPtr() == NULL) {
            ErrString() = std::string("Store::GetPtr : ") + _store.ErrString();
            return false;
        }
        _ptr_state = (StateType *)_store.GetPtr();
        return true;
    }

    // initiate storage
    bool InitStorage() {
        if (!IsBindStorage()) return false;

        _ptr_state->Init(_store.GetSize());
        return true;
    }

    // accessor funcs
    size_type Size() const {
        if (!IsBindStorage()) return 0;
        return _ptr_state->Size();
    }

    size_type MaxSize() const {
        if (!IsBindStorage()) return 0; 
        return _ptr_state->MaxSize();
    }

    bool IsFull()  const { return _ptr_state->IsFull();  }
    bool IsEmpty() const { return _ptr_state->IsEmpty(); }
    bool IsBindStorage() const { return _ptr_state != NULL; }

    // static funcs
    static size_t CalcByteSize(size_type size) { return StateType::CalcByteSize(size); }

protected:
    StateType*     _ptr_state;
    StoreType      _store;
};

} // end of namespace detail
} // end of namespace container
} // end of namespace lsf

// vim:ts=4:sw=4:et:ft=cpp:
