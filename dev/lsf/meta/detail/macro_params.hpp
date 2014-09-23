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

// tuple params
#define LSF_TUPLE_DATA_CTOR(n, p) \
    TupleData(LSF_REPEAT(n, _LSF_TUPLE_DATA_CALL_PARAM, _LSF_TUPLE_DATA_CALL_PARAM_END, p)) : head(LSF_TOKEN_CAT(p,1)), tail(LSF_ARGS_S(LSF_CNT_DEC(n),p)) { }

#define LSF_TUPLE_CTOR(n,p) \
    Tuple(LSF_REPEAT(n, _LSF_TUPLE_CALL_PARAM, _LSF_TUPLE_CALL_PARAM_END, p)) : base_type(LSF_ARGS(n,p)) { }

#define _LSF_TUPLE_DATA_CALL_PARAM(n,p)         typename detail::TupleParam<L,N+n>::type p##n,
#define _LSF_TUPLE_DATA_CALL_PARAM_END(n,p)     typename detail::TupleParam<L,N+n>::type p##n
#define _LSF_TUPLE_CALL_PARAM(n,p)      typename detail::TupleParam<list_type,n>::type p##n,
#define _LSF_TUPLE_CALL_PARAM_END(n,p)  typename detail::TupleParam<list_type,n>::type p##n
	
//// func params
//#define LSF_FUNC_PARAMS(n, p) LSF_REPEAT(n, __LSF_FUNC_PARAM, __LSF_FUNC_PARAM_END, p)
//#define __LSF_FUNC_PARAM(n,t) T##n t##n, 
//#define __LSF_FUNC_PARAM_END(n,t) T##n t##n

//// func pparams
//#define LSF_FUNC_PPARAMS(n, p) LSF_REPEAT(n, __LSF_FUNC_PPARAM, __LSF_FUNC_PPARAM_END, p)
//#define __LSF_FUNC_PPARAM(n,t) P##n t##n, 
//#define __LSF_FUNC_PPARAM_END(n,t) P##n t##n

//// enum items
//#define LSF_ENUM_ITEMS(n, t) LSF_REPEAT(n,LSF_ARG,LSF_ARG_END,t)

// vim:ts=4:sw=4:et:ft=cpp:
