// File:        macro_params.hpp
// Description: ---
// Notes:       ---
// Author:      leoxiang <leoxiang@tencent.com>
// Revision:    2012-11-07 by leoxiang

#pragma once

// template params
#define LSF_TPARAMS(n)                  LSF_REPEAT(n,__LSF_TPARAM,    __LSF_TPARAM_END,    T)
#define LSF_TPARAMS_DEF(n, def)         LSF_REPEAT(n,__LSF_TPARAM_DEF,__LSF_TPARAM_DEF_END,def)
#define __LSF_TPARAM(n, t)              typename t##n, 
#define __LSF_TPARAM_END(n, t)          typename t##n
#define __LSF_TPARAM_DEF(n, def)        typename T##n = def, 
#define __LSF_TPARAM_DEF_END(n, def)    typename T##n = def

// args
#define LSF_ARGS(n, t)          LSF_REPEAT(n,__LSF_ARG,  __LSF_ARG_END,  t)
#define LSF_ARGS_S(n, t)        LSF_REPEAT(n,__LSF_ARG_S,__LSF_ARG_S_END,t)
#define __LSF_ARG(n, t)         t##n, 
#define __LSF_ARG_END(n, t)     t##n
#define __LSF_ARG_S(n, t)       LSF_TOKEN_CAT(t,LSF_CNT_INC_##n),
#define __LSF_ARG_S_END(n, t)   LSF_TOKEN_CAT(t,LSF_CNT_INC_##n)

// typedefs
#define LSF_TYPEDEFS(n)                 LSF_REPEAT(n,__LSF_TYPEDEF,  __LSF_TYPEDEF_END,  T)
#define LSF_TYPEDEFS_D(n, t)            LSF_REPEAT(n,__LSF_TYPEDEF_D,__LSF_TYPEDEF_D_END,t)
#define __LSF_TYPEDEF(n, t)             typedef t##n type##n;
#define __LSF_TYPEDEF_END(n, t)         typedef t##n type##n;
#define __LSF_TYPEDEF_D(n, t)           typedef t type##n;
#define __LSF_TYPEDEF_D_END(n, t)       typedef t type##n;

// list items
#define LSF_LIST_ITEMS(n, t)        LSF_REPEAT(n,__LSF_LIST_ITEM,__LSF_LIST_ITEM_END,t)
#define __LSF_LIST_ITEM(n, t)       t,
#define __LSF_LIST_ITEM_END(n, t)   t

// vim:ts=4:sw=4:et:ft=cpp:
