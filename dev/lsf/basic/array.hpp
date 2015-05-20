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
        _size = std::min(SIZE, rhs.Size());
        std::copy(rhs.Data(), rhs.Data() + _size, Data());
    }

    template<typename OtherType, size_t OTHER_SIZE>
    Array(Array<OtherType, OTHER_SIZE> const & rhs)
    {
        _size = std::min(SIZE, rhs.Size());
        std::copy(rhs.Data(), rhs.Data() + _size, Data());
    }

    template<typename IterType>
    Array(IterType iter, size_t size)
    { 
        _size = size;
        std::copy(iter, iter+size, Begin());
    }

    ~Array() { }

    // assign operator
    Array<ElemType, SIZE> & operator=(Array<ElemType, SIZE> const & rhs) {
        if (this == &rhs) return *this;

        _size = std::min(SIZE, rhs.Size());
        std::copy(rhs.Begin(), rhs.End(), Data());
        return *this;
    }

    template<typename OtherType, size_t OTHER_SIZE>
    Array<ElemType, SIZE> & operator=(Array<OtherType, OTHER_SIZE> const & rhs) {
        if (this == &rhs) return *this;

        _size = std::min(SIZE, rhs.Size());
        std::copy(rhs.Data(), rhs.Data() + _size, Data());
        return *this;
    }

    // modifier
    template<typename IterType>
    void Copy(IterType it_begin, IterType it_end) {
        _size = it_end - it_begin;
        std::copy(it_begin, it_end, Begin());
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

    iterator Erase(iterator begin, iterator end) {
        if (begin < Begin() || begin >= End()) return End();
        if (end < Begin() || end > End()) return End();
        if (begin > end) return End();
        std::copy(end , End(), begin);
        _size -= (end - begin);
        return begin;
    }

    void SetSize(size_t size) { _size = std::min(SIZE, size); }

    void Reset() { _size = 0; }

    void Fill(value_type & val) { _size = SIZE; std::fill(Begin(), End(), val); }
    void Fill(value_type   val) { _size = SIZE; std::fill(Begin(), End(), val); }

    // member funcs
    iterator       Begin()       { return _array; }
    const_iterator Begin() const { return _array; }
    reverse_iterator       RBegin()       { return reverse_iterator(End()); }
    const_reverse_iterator RBegin() const { return const_reverse_iterator(End()); }

    iterator       End()       { return _array + _size; }
    const_iterator End() const { return _array + _size; }
    reverse_iterator       REnd()       { return reverse_iterator(Begin()); }
    const_reverse_iterator REnd() const { return const_reverse_iterator(Begin()); }
    
    const_reference operator[](size_t idx) const { return _array[idx]; }
    reference       operator[](size_t idx)       { return _array[idx]; }

    value_type *        Data() { return _array; }
    value_type const *  Data() const { return _array; }

    size_t Size() const { return _size; }

    bool IsEmpty() const { return _size == 0; }
    bool IsFull()  const { return _size == SIZE; }

    template<typename OtherType, size_t OTHER_SIZE>
    bool operator==(Array<OtherType, OTHER_SIZE> const & rhs) {
        return std::equal(Begin(), End(), rhs.Begin());
    }

    template<typename OtherType, size_t OTHER_SIZE>
    bool operator!=(Array<OtherType, OTHER_SIZE> const & rhs) {
        return !(*this == rhs);
    }

    std::string ToString(std::string const & delimit = ",") const {
        std::string tmp;
        for (const_iterator it = Begin(); it != End(); ++it) {
            tmp += it->ToString();
            if ((it+1) != End()) tmp += delimit;
        }
        return tmp;
    }

protected:
    size_t          _size;
    value_type      _array[SIZE];
};

} // End of namespace basic
} // End of namespace lsf

// vim:ts=4:sw=4:et:ft=cpp:
