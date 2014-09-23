// File:        buffer.hpp
// Description: ---
// Notes:       ---
// Author:      leoxiang <leoxiang@tencent.com>
// Revision:    2012-06-02 by leoxiang

#pragma once

#include <cstring>
#include <string>
#include <cstdio>
#include <algorithm>

namespace lsf {
namespace basic {

////////////////////////////////////////////////////////////
// forward declare
class Buffer;
static std::string BinToString(void const * input, size_t inlen);
static std::string BinToHexString(void const * input, size_t inlen);
static Buffer HexStringToBin(std::string input);

////////////////////////////////////////////////////////////
// Buffer: simple replacement of binary buffer
class Buffer
{
public:
    typedef     uint8_t *           iterator;
    typedef     uint8_t const *     const_iterator;

public:
    ////////////////////////////////////////////////////////////
    // constructor
    Buffer() : _size(0), _max_size(0), _ptr_data(NULL) 
    { }

    Buffer(size_t size) : _size(0), _max_size(0), _ptr_data(NULL) { 
        Init(size); 
    }

    Buffer(Buffer const & rhs) : _size(0), _max_size(0), _ptr_data(NULL) {
        Copy(rhs._ptr_data, rhs._size);
    }

    // do not use explicit to allow auto type conversion
    Buffer(void const * ptr, size_t size) : _size(0), _max_size(0), _ptr_data(NULL) {
        Copy(ptr, size);
    }

    ~Buffer() { Release(); }

    Buffer & operator=(Buffer const & rhs) {
        if (this == &rhs) return *this;

        Copy(rhs._ptr_data, rhs._size);
        return *this;
    }

    void Init(size_t size) {
        Release();
        _size = 0;
        _max_size = size;
        _ptr_data = (uint8_t *)::malloc(size);
    }

    void Release() {
        _size = 0;
        _max_size = 0;
        if (_ptr_data != NULL) ::free(_ptr_data);
        _ptr_data = NULL;
    }

    ////////////////////////////////////////////////////////////
    // common funcs
    void Copy(void const * ptr, size_t size) { 
        if (_max_size < size) {
            Release();
            Init(size);
        }
        memcpy(_ptr_data, ptr, size); 
        _size = size;
    }

    uint8_t &       operator[](size_t index) { return *(_ptr_data + index); }
    uint8_t const & operator[](size_t index) const { return *(_ptr_data + index); }

    //support implicit convertion to pointer type
    operator uint8_t const *() const { return _ptr_data; }
    operator uint8_t *() { return _ptr_data; }

    // operator
    bool operator==(Buffer const & rhs) {
        return (rhs._size == _size) && 
            memcmp(_ptr_data, rhs._ptr_data, std::min(rhs._size, _size)) == 0;
    }

    // accessor
    size_t Size()    const { return _size; }
    size_t MaxSize() const { return _max_size; }
    uint8_t *       Data() { return _ptr_data; }
    uint8_t const * Data() const { return _ptr_data; }

    iterator       Begin() { return _ptr_data; }
    iterator       End()   { return _ptr_data + _size; }
    const_iterator Begin() const { return _ptr_data; }
    const_iterator End()   const { return _ptr_data + _size; }

    std::string ToHexString() const { return BinToHexString(_ptr_data, _size); }
    std::string ToString()    const { return BinToHexString(_ptr_data, _size); }

    bool IsString() const { return std::find(Begin(), End(), '\0') != End(); }

    friend Buffer HexStringToBin(std::string input);

private:
    size_t      _size;
    size_t      _max_size;
    uint8_t *   _ptr_data;
};

////////////////////////////////////////////////////////////
// BufferRef: no copy action compared to buffer
class BufferRef
{
public:
    ////////////////////////////////////////////////////////////
    // constructor
    BufferRef() : _size(0), _ptr_data(NULL) 
    { }

    BufferRef(BufferRef const & rhs) : _size(rhs._size), _ptr_data(rhs._ptr_data)
    { }

    BufferRef(void * ptr, size_t size) : _size(size), _ptr_data((uint8_t *)ptr) 
    { }

    BufferRef & operator=(BufferRef const & rhs) {
        if (this == &rhs) return *this;

        _size = rhs._size;
        _ptr_data = rhs._ptr_data;
        return *this;
    }

    // common funcs
    uint8_t & operator[](size_t index) const { return *(_ptr_data + index); }

    bool IsString() const { return std::find(_ptr_data, _ptr_data + _size, '\0') != _ptr_data + _size; }

    // accessor
    size_t    Size() const { return _size; }

    uint8_t *       Data()       { return _ptr_data; }
    uint8_t const * Data() const { return _ptr_data; }

    Buffer      ToBuffer()    const { return Buffer(_ptr_data, _size); }
    std::string ToHexString() const { return BinToHexString(_ptr_data, _size); }
    std::string ToString()    const { return BinToHexString(_ptr_data, _size); }

private:
    size_t              _size;
    mutable uint8_t *   _ptr_data;
};

////////////////////////////////////////////////////////////
// static function
static inline std::string BinToHexString(void const * input, size_t inlen)
{
    static const char CHAR_TABLE[] = {
        '0', '1', '2', '3', '4', '5', '6', '7', 
        '8', '9', 'a', 'b', 'c', 'd', 'e', 'f' };
    std::string tmp;

    for (size_t off = 0; off < inlen; off++) {
        tmp.push_back(CHAR_TABLE[(*((unsigned char *)input + off) >> 4) & 0xf]);
        tmp.push_back(CHAR_TABLE[*((unsigned char *)input + off) & 0xf]);
    }
    return tmp;
}

static inline std::string BinToString(void const * input, size_t inlen) 
{
    std::string tmp;

    for (size_t off = 0; off < inlen; off++) {
        unsigned char ch = *((unsigned char *)input + off);
        if (ch < 0x20 || ch > 0x7e) 
            tmp.push_back('.');
        else
            tmp.push_back(ch);
    }
    return tmp;
}

static inline Buffer HexStringToBin(std::string input) 
{
    Buffer buf(input.size() / 2);

    for (size_t off = 0; off < input.size() / 2; off++) {
        switch (input[off * 2]) {
            case '1': buf[off] = (0x1 << 4) & 0xf0; break;
            case '2': buf[off] = (0x2 << 4) & 0xf0; break;
            case '3': buf[off] = (0x3 << 4) & 0xf0; break;
            case '4': buf[off] = (0x4 << 4) & 0xf0; break;
            case '5': buf[off] = (0x5 << 4) & 0xf0; break;
            case '6': buf[off] = (0x6 << 4) & 0xf0; break;
            case '7': buf[off] = (0x7 << 4) & 0xf0; break;
            case '8': buf[off] = (0x8 << 4) & 0xf0; break;
            case '9': buf[off] = (0x9 << 4) & 0xf0; break;
            case 'a': buf[off] = (0xa << 4) & 0xf0; break;
            case 'b': buf[off] = (0xb << 4) & 0xf0; break;
            case 'c': buf[off] = (0xc << 4) & 0xf0; break;
            case 'd': buf[off] = (0xd << 4) & 0xf0; break;
            case 'e': buf[off] = (0xe << 4) & 0xf0; break;
            case 'f': buf[off] = (0xf << 4) & 0xf0; break;
            case 'A': buf[off] = (0xa << 4) & 0xf0; break;
            case 'B': buf[off] = (0xb << 4) & 0xf0; break;
            case 'C': buf[off] = (0xc << 4) & 0xf0; break;
            case 'D': buf[off] = (0xd << 4) & 0xf0; break;
            case 'E': buf[off] = (0xe << 4) & 0xf0; break;
            case 'F': buf[off] = (0xf << 4) & 0xf0; break;
        }
        switch (input[off * 2 + 1]) {
            case '1': buf[off] += 0x1; break;
            case '2': buf[off] += 0x2; break;
            case '3': buf[off] += 0x3; break;
            case '4': buf[off] += 0x4; break;
            case '5': buf[off] += 0x5; break;
            case '6': buf[off] += 0x6; break;
            case '7': buf[off] += 0x7; break;
            case '8': buf[off] += 0x8; break;
            case '9': buf[off] += 0x9; break;
            case 'a': buf[off] += 0xa; break;
            case 'b': buf[off] += 0xb; break;
            case 'c': buf[off] += 0xc; break;
            case 'd': buf[off] += 0xd; break;
            case 'e': buf[off] += 0xe; break;
            case 'f': buf[off] += 0xf; break;
            case 'A': buf[off] += 0xa; break;
            case 'B': buf[off] += 0xb; break;
            case 'C': buf[off] += 0xc; break;
            case 'D': buf[off] += 0xd; break;
            case 'E': buf[off] += 0xe; break;
            case 'F': buf[off] += 0xf; break;
        }
        buf._size++;
    }
    return buf;
}

} // End of namespace basic
} // End of namespace lsf

// vim:ts=4:sw=4:et:ft=cpp:
