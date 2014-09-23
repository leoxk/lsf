// File:        assert.hpp
// Description: ---
// Notes:       ---
// Author:      leoxiang <leoxiang@tencent.com>
// Revision:    2011-09-13 by leoxiang

#pragma once

#include <iostream>
#include <cstring>
#include <cerrno>

////////////////////////////////////////////////////////////
// ASSERT macros
#ifdef NDEBUG
    
#   define ASSERT(expr)
#   define ASSERT_PERROR(errno)
#   define ASSERT_VERBOSE(errno)

#else
    
#   define ASSERT(expr)   \
    do \
    { \
        if (!(expr)) \
            std::cerr << "\e[1m\e[31m" "[ASSERT]" " Failed: " "\e[m" << "(" << _STR(expr) << ")" \
                        << __FILE__ << ":" << __LINE__ << ":" << __func__ << std::endl; \
    } while(0)

#   define ASSERT_VERBOSE(expr)   \
    do \
    { \
        if (!(expr)) \
            std::cerr << "\e[1m\e[31m" "[ASSERT_VERBOSE]" " Failed: " "\e[m" << "(" << _STR(expr) << ")" \
                        << __FILE__ << ":" << __LINE__ << ":" << __func__ << std::endl; \
        else \
            std::cout << "\e[1m\e[32m" "[ASSERT_VERBOSE]" " Passed: " "\e[m" << "(" << _STR(expr) << ")" \
                        << __FILE__ << ":" << __LINE__ << ":" << __func__ << std::endl; \
    } while(0)
#endif

////////////////////////////////////////////////////////////
// some helper funcs
#define BOOT_ASSERT(info, expr) \
    do \
    { \
        std::cerr << std::setw(50) << std::left << info; \
        if (!(expr)) \
            std::cerr << "\e[1m\e[31m" "[Fail]" "\e[m" << std::endl; \
        else \
            std::cerr << "\e[1m\e[32m" "[Pass]" "\e[m" << std::endl; \
    } while (0)

#define CHK_RET(expr) \
    do { \
        if (expr) { \
            std::cout << strerror(errno) << ":" << __FILE__ \
                                    << ":" << __LINE__ \
                                    << ":" << __func__ << std::endl; \
        } \
    } while (0)

#define CHK_RET_EXIT(expr) \
    do { \
        if (expr) { \
            std::cout << strerror(errno) << ":" << __FILE__ \
                                    << ":" << __LINE__ \
                                    << ":" << __func__ << std::endl; \
            exit(EXIT_FAILURE); \
        } \
    } while (0)

#define CHK_ERR() \
    do { \
        std::cout << strerror(errno) << ":" << __FILE__ \
                                << ":" << __LINE__ \
                                << ":" << __func__ << std::endl; \
    } while (0)

////////////////////////////////////////////////////////////
// define compile time ASSERT
// token to string
#define __STR(s)             #s
#define _STR(s)              __STR(s)
// cat string token
#define __CAT(s1, s2)        s1##s2
#define _CAT(s1, s2)         __CAT(s1,s2)
/// compile time assert
#define STATIC_ASSERT(x)  \
        enum {_CAT(STATIC_ASSERT_FAILED_AT_LINE_, __LINE__) = 1 / !!(x)};

////////////////////////////////////////////////////////////
// 64bit byte order convert
#if   __BYTE_ORDER == __BIG_ENDIAN
#   define NTOHLL(x) (x)
#elif __BYTE_ORDER == __LITTLE_ENDIAN
#   define NTOHLL(x)  \
        ((((x) & 0xff00000000000000llu) >> 56) | \
         (((x) & 0x00ff000000000000llu) >> 40) | \
         (((x) & 0x0000ff0000000000llu) >> 24) | \
         (((x) & 0x0000ff0000000000llu) >> 24) | \
         (((x) & 0x000000ff00000000llu) >> 8)  | \
         (((x) & 0x00000000ff000000llu) << 8)  | \
         (((x) & 0x0000000000ff0000llu) << 24) | \
         (((x) & 0x000000000000ff00llu) << 40) | \
         (((x) & 0x00000000000000ffllu) << 56) )  
#else
#   error "unknown system"
#endif
#define HTONLL(x) NTOHLL(x)

#define NTOH64(v) (uint64_t)(NTOHLL(v))
#define NTOH32(v) (uint32_t)(ntohl(v))
#define NTOH16(v) (uint16_t)(ntohs(v))
#define HTON64(v) (uint64_t)(HTONLL(v))
#define HTON32(v) (uint32_t)(htonl(v))
#define HTON16(v) (uint16_t)(htons(v))

// vim:ts=4:sw=4:et:ft=cpp:
