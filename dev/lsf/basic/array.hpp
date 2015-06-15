// File:        array.hpp
// Description: ---
// Notes:       ---
// Author:      leoxiang <leoxiang727@qq.com>
// Revision:    2012-06-01 by leoxiang

#pragma once

#include <stdint.h>
#include <cstring>
#include <iterator>
#include <algorithm>

namespace lsf {
namespace basic {
    
////////////////////////////////////////////////////////////
// Array, static version of Array, size is specified @ compile time
template<typename ElemType, size_t SIZE>
class Array 
{
public:
    typedef ElemType           value_type;
    typedef ElemType *         iterator;
    typedef ElemType const *   const_iterator;
    typedef ElemType &         reference;
    typedef ElemType const &   const_reference;
    typedef std::reverse_iterator<iterator>         reverse_iterator;
    typedef std::reverse_iterator<const_iterator>   const_reverse_iterator;

public:
    // constructor
    Array(size_t size = 0) : _size(size) { }

    Array(Array<ElemType, SIZE> const & rhs)
    {
        _size = std::min(SIZE, rhs.size());
        std::copy(rhs.data(), rhs.data() + _size, data());
    }

    template<typename OtherType, size_t OTHER_SIZE>
    Array(Array<OtherType, OTHER_SIZE> const & rhs)
    {
        _size = std::min(SIZE, rhs.size());
        std::copy(rhs.data(), rhs.data() + _size, data());
    }

    template<typename IterType>
    Array(IterType iter, size_t size)
    { 
        _size = size;
        std::copy(iter, iter+size, begin());
    }

    ~Array() { }

    // assign operator
    Array<ElemType, SIZE> & operator=(Array<ElemType, SIZE> const & rhs) {
        if (this == &rhs) return *this;

        _size = std::min(SIZE, rhs.size());
        std::copy(rhs.begin(), rhs.end(), data());
        return *this;
    }

    template<typename OtherType, size_t OTHER_SIZE>
    Array<ElemType, SIZE> & operator=(Array<OtherType, OTHER_SIZE> const & rhs) {
        if (this == &rhs) return *this;

        _size = std::min(SIZE, rhs.size());
        std::copy(rhs.data(), rhs.data() + _size, data());
        return *this;
    }

    // modifier
    template<typename IterType>
    void Copy(IterType it_begin, IterType it_end) {
        _size = it_end - it_begin;
        std::copy(it_begin, it_end, begin());
    }

    template<typename OtherType>
    bool PushBack(OtherType const & value) {
        if (_size >= SIZE) return false;
        _array[_size++] = value;
        return true;
    }

    iterator Erase(iterator pos) {
        return Erase(pos, pos+1);
    }

    iterator Erase(iterator begin_iter, iterator end_iter) {
        if (begin_iter < begin() || begin_iter >= end()) return end();
        if (end_iter < begin() || end_iter > end()) return end();
        if (begin_iter > end_iter) return end();
        std::copy(end_iter , end(), begin_iter);
        _size -= (end_iter - begin_iter);
        return begin_iter;
    }

    void SetSize(size_t size) { _size = std::min(SIZE, size); }

    void Reset() { _size = 0; }

    void Fill(value_type & val) { _size = SIZE; std::fill(begin(), end(), val); }
    void Fill(value_type   val) { _size = SIZE; std::fill(begin(), end(), val); }

    // member funcs
    iterator       begin()       { return _array; }
    const_iterator begin() const { return _array; }
    reverse_iterator       rbegin()       { return reverse_iterator(end()); }
    const_reverse_iterator rbegin() const { return const_reverse_iterator(end()); }

    iterator       end()       { return _array + _size; }
    const_iterator end() const { return _array + _size; }
    reverse_iterator       rend()       { return reverse_iterator(begin()); }
    const_reverse_iterator rend() const { return const_reverse_iterator(begin()); }
    
    const_reference operator[](size_t idx) const { return _array[idx]; }
    reference       operator[](size_t idx)       { return _array[idx]; }

    value_type *        data() { return _array; }
    value_type const *  data() const { return _array; }

    size_t size() const { return _size; }
    size_t max_size() const { return SIZE; }

    bool empty() const { return _size == 0; }
    bool full()  const { return _size == SIZE; }

    template<typename OtherType, size_t OTHER_SIZE>
    bool operator==(Array<OtherType, OTHER_SIZE> const & rhs) {
        return std::equal(begin(), end(), rhs.begin());
    }

    template<typename OtherType, size_t OTHER_SIZE>
    bool operator!=(Array<OtherType, OTHER_SIZE> const & rhs) {
        return !(*this == rhs);
    }

    std::string ToString(std::string const & delimit = ",") const {
        std::string tmp;
        for (const_iterator it = begin(); it != end(); ++it) {
            tmp += it->ToString();
            if ((it+1) != end()) tmp += delimit;
        }
        return tmp;
    }

protected:
    size_t          _size;
    value_type      _array[SIZE];
};

} // end of namespace basic
} // end of namespace lsf

// vim:ts=4:sw=4:et:ft=cpp:
