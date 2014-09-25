// File:        array.hpp
// Description: ---
// Notes:       ---
// Author:      leoxiang <leoxiang@tencent.com>
// Revision:    2012-06-01 by leoxiang

#pragma once

#include <stdint.h>
#include <cstring>
#include <iterator>
#include <algorithm>

namespace lsf {
namespace basic {
    
////////////////////////////////////////////////////////////
// Array, single replace of c/c++ built-in array
template<typename ElemType>
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
    Array(size_t size = 0) : _size(0), _ptr_array_head(NULL) { 
        Init(size); 
    }

    Array(Array<ElemType> const & rhs) : _size(0), _ptr_array_head(NULL) {
        Init(rhs._size);
        std::copy(rhs.Begin(), rhs.End(), Begin());
    }

    template<typename OtherType>
    Array(Array<OtherType> const & rhs) : _size(0), _ptr_array_head(NULL) {
        Init(rhs._size);
        std::copy(rhs.Begin(), rhs.End(), Begin());
    }

    template<typename IterType>
    Array(IterType iter, size_t size) : _size(0), _ptr_array_head(NULL) {
        Init(size);
        std::copy(iter, iter + size, Begin());
    }

    ~Array() { Release(); }

    Array<ElemType> & operator=(Array<ElemType> const & rhs) {
        if (this == &rhs) return *this;

        Init(rhs._size);
        std::copy(rhs.Begin(), rhs.End(), Begin());
        return *this;
    }

    template<typename OtherType>
    Array<ElemType> & operator=(Array<OtherType> const & rhs) {
        if (this == &rhs) return *this;

        Init(rhs._size);
        std::copy(rhs.Begin(), rhs.End(), Begin());
        return *this;
    }

    void Init(size_t size) {
        Release();
        _size = size;
        if (_size != 0) _ptr_array_head = ::new value_type[_size];
    }

    void Release() {
        _size = 0;
        if (_ptr_array_head != NULL) ::delete[] _ptr_array_head;
        _ptr_array_head = NULL;
    }

    template<typename IterType>
    void Copy(IterType it_begin, IterType it_end) {
        Init(it_end - it_begin);
        std::copy(it_begin, it_end, Begin());
    }

    // member funcs
    iterator       Begin()       { return _ptr_array_head; }
    const_iterator Begin() const { return _ptr_array_head; }
    reverse_iterator       RBegin()       { return reverse_iterator(End()); }
    const_reverse_iterator RBegin() const { return const_reverse_iterator(End()); }

    iterator       End()       { return _ptr_array_head + _size; }
    const_iterator End() const { return _ptr_array_head + _size; }
    reverse_iterator       REnd()       { return reverse_iterator(Begin()); }
    const_reverse_iterator REnd() const { return const_reverse_iterator(Begin()); }
    
    const_reference operator[](size_t idx) const { return _ptr_array_head[idx]; }
    reference       operator[](size_t idx)       { return _ptr_array_head[idx]; }

    value_type *        Data() { return _ptr_array_head; }
    value_type const *  Data() const { return _ptr_array_head; }

    void Fill(value_type & val) { std::fill(Begin(), End(), val); }
    void Fill(value_type   val) { std::fill(Begin(), End(), val); }

    template<typename OtherType>
    bool operator==(Array<OtherType> const & rhs) {
        if (_size != rhs._size) return false;
        return std::equal(Begin(), End(), rhs.Begin());
    }

    template<typename OtherType>
    bool operator!=(Array<OtherType> const & rhs) {
        return !(*this == rhs);
    }

    // static funcs
    size_t ByteSize() const { return _size * sizeof(value_type); }
    size_t Size()     const { return _size; }

protected:
    size_t          _size;
    value_type *    _ptr_array_head;
};

////////////////////////////////////////////////////////////
// StaticArray, static version of Array, size is specified @ compile time
template<typename ElemType, size_t SIZE>
class StaticArray 
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
    StaticArray(size_t size = 0) : _size(size) { }

    StaticArray(StaticArray<ElemType, SIZE> const & rhs)
    {
        _size = std::min(SIZE, rhs.Size());
        std::copy(rhs.Data(), rhs.Data() + _size, Data());
    }

    template<typename OtherType, size_t OTHER_SIZE>
    StaticArray(StaticArray<OtherType, OTHER_SIZE> const & rhs)
    {
        _size = std::min(SIZE, rhs.Size());
        std::copy(rhs.Data(), rhs.Data() + _size, Data());
    }

    ~StaticArray() { }

    StaticArray<ElemType, SIZE> & operator=(StaticArray<ElemType, SIZE> const & rhs) {
        if (this == &rhs) return *this;

        _size = std::min(SIZE, rhs.Size());
        std::copy(rhs.Begin(), rhs.End(), Data());
        return *this;
    }

    template<typename OtherType, size_t OTHER_SIZE>
    StaticArray<ElemType, SIZE> & operator=(StaticArray<OtherType, OTHER_SIZE> const & rhs) {
        if (this == &rhs) return *this;

        _size = std::min(SIZE, rhs.Size());
        std::copy(rhs.Data(), rhs.Data() + _size, Data());
        return *this;
    }

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

    void SetSize(size_t size) { _size = std::min(SIZE, size); }

    // member funcs
    iterator       Begin()       { return _array; }
    const_iterator Begin() const { return _array; }
    reverse_iterator       RBegin()       { return reverse_iterator(End()); }
    const_reverse_iterator RBegin() const { return const_reverse_iterator(End()); }

    iterator       End()       { return _array + SIZE; }
    const_iterator End() const { return _array + SIZE; }
    reverse_iterator       REnd()       { return reverse_iterator(Begin()); }
    const_reverse_iterator REnd() const { return const_reverse_iterator(Begin()); }
    
    const_reference operator[](size_t idx) const { return _array[idx]; }
    reference       operator[](size_t idx)       { return _array[idx]; }

    value_type *        Data() { return _array; }
    value_type const *  Data() const { return _array; }

    size_t Size() const { return _size; }

    void Fill(value_type & val) { _size = SIZE; std::fill(Begin(), End(), val); }
    void Fill(value_type   val) { _size = SIZE; std::fill(Begin(), End(), val); }

    template<typename OtherType>
    bool operator==(StaticArray<OtherType, SIZE> const & rhs) {
        return std::equal(Begin(), End(), rhs.Begin());
    }

    template<typename OtherType>
    bool operator!=(StaticArray<OtherType, SIZE> const & rhs) {
        return !(*this == rhs);
    }

protected:
    size_t          _size;
    value_type      _array[SIZE];
};

} // End of namespace basic
} // End of namespace lsf

// vim:ts=4:sw=4:et:ft=cpp:
