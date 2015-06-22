// File:        macro.hpp
// Description: ---
// Notes:       ---
// Author:      leoxiang <leoxiang727@qq.com>
// Revision:    2012-12-10 by leoxiang

#pragma once

// token to string
#define LSF_TOKEN_CAT(x, y) __LSF_TOKEN_CAT1(x, y)
#define LSF_TOKEN_TO_STRING(x) __LSF_TOKEN_TO_STRING1(x)
#define __LSF_TOKEN_CAT1(x, y) x##y
#define __LSF_TOKEN_TO_STRING1(x) #x

// debug
#define LSF_DEBUG_INFO "[" __FILE__ "|" LSF_TOKEN_TO_STRING(__LINE__) "] "

// short cut for c_str()
#define ToCharStr() ToString().c_str()
#define ErrCharStr() ErrString().c_str()
#define SysErrCharStr() SysErrString().c_str()

// vim:ts=4:sw=4:et:ft=cpp:
