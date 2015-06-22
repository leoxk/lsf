// File:        queue.hpp
// Description: ---
// Notes:
// Author:      leoxiang <leoxiang727@qq.com>
// Revision:    2012-05-10 by leoxiang

#pragma once

#include "lsf/container/detail/basic_container.hpp"
#include "lsf/container/detail/impl_circular_queue.hpp"
#include "lsf/container/shared_mem.hpp"

namespace lsf {
namespace container {

////////////////////////////////////////////////////////////
// Queue
////////////////////////////////////////////////////////////
template <typename ElemType, typename StoreType = SharedMem, typename SizeType = size_t>
class Queue : public detail::BasicContainer<ElemType, SizeType, StoreType, detail::QueueState<ElemType, SizeType>,
                                            detail::QueueIterator<ElemType, SizeType>> {
public:
    typedef detail::BasicContainer<ElemType, SizeType, StoreType, detail::QueueState<ElemType, SizeType>,
                                   detail::QueueIterator<ElemType, SizeType>> base_type;
    typedef typename base_type::value_type value_type;
    typedef typename base_type::size_type size_type;
    typedef typename base_type::iterator iterator;
    typedef typename base_type::reverse_iterator reverse_iterator;

public:
    bool PushFront(value_type const& val) {
        if (base_type::full()) return false;

        new (&(*--begin())) value_type(val);
        base_type::_ptr_state->HeadPosDec();
        return true;
    }

    bool PushBack(value_type const& val) {
        if (base_type::full()) return false;

        new (&*end()) value_type(val);
        base_type::_ptr_state->TailPosInc();
        return true;
    }

    bool PopFront() {
        if (base_type::empty()) return false;

        // call destructor
        GetFront()->~value_type();

        // increase head pos
        base_type::_ptr_state->HeadPosInc();
        return true;
    }

    bool PopBack() {
        if (base_type::empty()) return false;

        // call destructor
        GetBack()->~value_type();

        // decrease tail pos
        base_type::_ptr_state->TailPosDec();
        return true;
    }

    // accessor
    iterator GetFront() { return iterator(base_type::_ptr_state, 0); }
    iterator GetBack() { return iterator(base_type::_ptr_state, std::max((int)base_type::size() - 1, 0)); }

    iterator begin() { return iterator(base_type::_ptr_state, 0); }
    iterator end() { return iterator(base_type::_ptr_state, base_type::size()); }

    reverse_iterator rbegin() { return reverse_iterator(end()); }
    reverse_iterator rend() { return reverse_iterator(begin()); }
};

}  // end of namespace container
}  // end of namespace lsf

// vim:ts=4:sw=4:et:ft=cpp:
