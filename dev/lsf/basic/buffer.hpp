// File:        buffer.hpp
// Description: ---
// Notes:       ---
// Author:      leoxiang <leoxiang727@qq.com>
// Revision:    2012-06-02 by leoxiang

#pragma once

#include <cstring>
#include <string>
#include <cstdio>
#include <algorithm>
#include "lsf/basic/array.hpp"

namespace lsf {
namespace basic {

////////////////////////////////////////////////////////////
// forward declare
static std::string BinToString(void const * input, size_t inlen);
static std::string BinToHexString(void const * input, size_t inlen);

////////////////////////////////////////////////////////////
// Buffer: A wrapper for Array with uint8_t as its item
template<size_t SIZE>
class Buffer : public Array<uint8_t, SIZE>
{ 
public:
    typedef Array<uint8_t, SIZE> base_type;

public:
    Buffer(size_t size = 0) : base_type(size) { }

    template<typename OtherType, size_t OTHER_SIZE>
    Buffer(Array<OtherType, OTHER_SIZE> const & rhs) : base_type(rhs) { }

    template<typename IterType>
    Buffer(IterType iter, size_t size) : base_type(iter, size) { }

    template<size_t OTHER_SIZE>
    Buffer & operator=(Buffer<OTHER_SIZE> const & rhs) { base_type::operator=(rhs); return *this; }

    std::string ToHexString()   const { return BinToHexString(base_type::Begin(), base_type::Size()); }

    std::string ToString()    const { return BinToString(base_type::Begin(), base_type::Size()); }

    void Copy(char const * str) { base_type::Copy(str, str + ::strlen(str)); }
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

template<size_t SIZE>
static inline bool HexStringToBin(std::string input, Buffer<SIZE> & buf) 
{
    buf.Reset();
    for (size_t off = 0; off < input.size() / 2; off++) 
    {
        buf.PushBack(0);
        switch (input[off * 2])
        {
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
        switch (input[off * 2 + 1])
        {
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
    }
    return true;
}

} // End of namespace basic
} // End of namespace lsf

// vim:ts=4:sw=4:et:ft=cpp:
