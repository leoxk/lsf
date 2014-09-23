// File:        serialize.hpp
// Description: ---
// Notes:       ---
// Author:      leoxiang <leoxiang@tencent.com>
// Revision:    2012-09-12 by leoxiang

#pragma once

#include <cstdlib>
#include <byteswap.h>
#include <sys/time.h>
#include <arpa/inet.h>
#include <cstring>

#if __BYTE_ORDER == __BIG_ENDIAN
#  define LSF_NTOHLL(x)       (x)
#  define LSF_HTONLL(x)       (x)
#elif __BYTE_ORDER == __LITTLE_ENDIAN
#  define LSF_NTOHLL(x)     __bswap_64 (x)
#  define LSF_HTONLL(x)     __bswap_64 (x)
#else
#  error "unknown system"
#endif

namespace lsf {
namespace util {

inline static void ConvertByteOrder(uint64_t & num) { num = LSF_NTOHLL(num); }
inline static void ConvertByteOrder(uint32_t & num) { num = ntohl(num); }
inline static void ConvertByteOrder(uint16_t & num) { num = ntohs(num); }
inline static void ConvertByteOrder(uint8_t  & num) { }

inline static void ConvertByteOrder(int64_t & num)  { num = LSF_NTOHLL(num); }
inline static void ConvertByteOrder(int32_t & num)  { num = ntohl(num); }
inline static void ConvertByteOrder(int16_t & num)  { num = ntohs(num); }
inline static void ConvertByteOrder(int8_t  & num)  { }

// Serialize
template<typename ElemType>
inline static bool Serialize(void * buf, size_t buflen, size_t & uselen, ElemType const & elem) {
    if (uselen + sizeof(elem) > buflen) return false;

    ElemType * pelem = (ElemType *)((uint8_t *)buf + uselen);
    //*pelem = elem;
    memcpy(pelem, &elem, sizeof(elem));
    ConvertByteOrder(*pelem);
    uselen = uselen + sizeof(elem);
    return true;
}

template<typename ElemType>
inline static bool Serialize(void * buf, size_t buflen, ElemType const & elem) {
    size_t uselen = 0;
    return Serialize(buf, buflen, uselen, elem);
}

// UnSerialize
template<typename ElemType>
inline static bool UnSerialize(void const * buf, size_t buflen, size_t & uselen, ElemType & elem) {
    if (uselen + sizeof(elem) > buflen) return false;

    ElemType * pelem = (ElemType *)((uint8_t *)buf + uselen);
    elem    = *pelem;
    ConvertByteOrder(elem);
    uselen  = uselen + sizeof(elem);
    return true;
}

template<typename ElemType>
inline static bool UnSerialize(void const * buf, size_t buflen, ElemType & elem) {
    size_t uselen = 0;
    return UnSerialize(buf, buflen, uselen, elem);
}

// SerializeBinary
inline static bool SerializeBinary(void * buf, size_t buflen, size_t & uselen, void const * bin, size_t binlen) {
    if (uselen + binlen > buflen) return false;
    ::memcpy((uint8_t *)buf + uselen, bin, binlen);
    uselen += binlen;
    return true;
}

inline static bool SerializeBinary(void * buf, size_t buflen, void const * bin, size_t binlen) {
    size_t uselen = 0;
    return SerializeBinary(buf, buflen, uselen, bin, binlen);
}

// UnSerializeBinary
inline static bool UnSerializeBinary(void const * buf, size_t buflen, size_t & uselen, void * bin, size_t binlen) {
    if (uselen + binlen > buflen) return false;
    ::memcpy(bin, (uint8_t *)buf + uselen, binlen);
    uselen += binlen;
    return true;
}

inline static bool UnSerializeBinary(void const * buf, size_t buflen, void * bin, size_t binlen) {
    size_t uselen = 0;
    return UnSerializeBinary(buf, buflen, uselen, bin, binlen);

#undef LSF_NTOHLL
#undef LSF_HTONLL

} // end of anonymous namespace
} // end of namespace util
} // end of namespace lsf

// vim:ts=4:sw=4:et:ft=cpp:
