// File:        serialize.hpp
// Description: ---
// Notes:       ---
// Author:      leoxiang <leoxiang727@qq.com>
// Revision:    2012-09-12 by leoxiang

#pragma once

#include <cstring>
#include <cstdint>

namespace lsf {
namespace util {

// template<typename ElemType>
// inline static bool Serialize(void * buf, size_t buflen, ElemType const & elem) {
// size_t uselen = 0;
// return Serialize(buf, buflen, uselen, elem);
//}

// template<typename ElemType>
// inline static bool UnSerialize(void const * buf, size_t buflen, ElemType & elem) {
// size_t uselen = 0;
// return UnSerialize(buf, buflen, uselen, elem);
//}

////////////////////////////////////////////////////////////
// Common
////////////////////////////////////////////////////////////
template <typename ElemType>
inline static bool Serialize(void *buf, size_t buflen, size_t &uselen, ElemType const &elem) {
    if (uselen + sizeof(elem) > buflen) return false;
    ElemType *pelem = (ElemType *)((uint8_t *)buf + uselen);
    *pelem = elem;
    uselen = uselen + sizeof(elem);
    return true;
}

template <typename ElemType>
inline static bool UnSerialize(void const *buf, size_t buflen, size_t &uselen, ElemType &elem) {
    if (uselen + sizeof(elem) > buflen) return false;
    ElemType *pelem = (ElemType *)((uint8_t *)buf + uselen);
    elem = *pelem;
    uselen = uselen + sizeof(elem);
    return true;
}

////////////////////////////////////////////////////////////
// Binary
////////////////////////////////////////////////////////////
inline static bool SerializeBinary(void *buf, size_t buflen, size_t &uselen, void const *bin, size_t binlen) {
    if (uselen + binlen > buflen) return false;
    ::memcpy((uint8_t *)buf + uselen, bin, binlen);
    uselen += binlen;
    return true;
}

inline static bool UnSerializeBinary(void const *buf, size_t buflen, size_t &uselen, void *bin, size_t binlen) {
    if (uselen + binlen > buflen) return false;
    ::memcpy(bin, (uint8_t *)buf + uselen, binlen);
    uselen += binlen;
    return true;
}

////////////////////////////////////////////////////////////
// Protobuf
////////////////////////////////////////////////////////////
template <typename PBType>
inline static bool SerializeProtobuf(void *buf, size_t buflen, size_t &uselen, PBType const &elem) {
    uint32_t protobuf_len = elem.ByteSize();
    if (!Serialize(buf, buflen, uselen, protobuf_len)) return false;

    if (uselen + protobuf_len > buflen) return false;
    if (!elem.SerializeWithCachedSizesToArray((uint8_t *)buf + uselen)) return false;
    uselen += protobuf_len;
    return true;
}

template <typename PBType>
inline static bool UnSerializeProtobuf(void const *buf, size_t buflen, size_t &uselen, PBType &elem) {
    uint32_t protobuf_len = 0;
    if (!lsf::util::UnSerialize(buf, buflen, uselen, protobuf_len)) return false;

    if (uselen + protobuf_len > buflen) return false;
    if (!elem.ParseFromArray((uint8_t *)buf + uselen, protobuf_len)) return false;
    uselen += protobuf_len;
    return true;
}

}  // end of namespace util
}  // end of namespace lsf

// vim:ts=4:sw=4:et:ft=cpp:
