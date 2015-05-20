// File:        type_list.hpp
// Description: ---
// Notes:       ---
// Author:      leoxiang <leoxiang727@qq.com>
// Revision:    2012-11-07 by leoxiang

#pragma once

#include <cstddef>
#include <stdexcept>
#include "lsf/basic/empty_type.hpp"
#include "lsf/meta/macro.hpp"
#include "lsf/meta/type_traits.hpp"

namespace lsf {
namespace meta {

////////////////////////////////////////////////////////////
// TypeList
//
// LSF_TPARAMS_DEF(5, def) expand like: 
// typename T1 = def, typename T2 = def, typename T3 = def, typename T4 = def, typename T5 = def
template< LSF_TPARAMS_DEF(LSF_MAX_TEMPLATE_PARAMS, basic::EmptyType) >
struct TypeList
{
    // define types
    //
    // LSF_TYPEDEFS(5) expand like:
    // typedef T1 type1; typedef T2 type2; typedef T3 type3; typedef T4 type4; typedef T5 type5;
    LSF_TYPEDEFS(LSF_MAX_TEMPLATE_PARAMS)

    // define tail type
    //
    // LSF_ARGS_S(LSF_CNT_DEC(5), T) expand like:
    // T2, T3, T4, T5
    typedef TypeList< LSF_ARGS_S(LSF_CNT_DEC(LSF_MAX_TEMPLATE_PARAMS), T) > tail;

    // recursively define size
    static const size_t size = 1 + tail::size;

    // recursively define largest type
    typedef typename SelectType< 
        sizeof(T1) >= sizeof(typename tail::largest_type),
        T1,
        typename tail::largest_type
        >::type largest_type;
};

////////////////////////////////////////////////////////////
// declare specialized TypeList
//
// LSF_LIST_ITEMS(5, type) expand like:
// type, type, type, type, type
template<>
struct TypeList< LSF_LIST_ITEMS(LSF_MAX_TEMPLATE_PARAMS, basic::EmptyType) >
{
    // define types
    //
    // LSF_TYPEDEFS_D(5, type) expand like:
    // typedef type type1; typedef type type2; typedef type type3; typedef type type4; typedef type type5;
     LSF_TYPEDEFS_D(LSF_MAX_TEMPLATE_PARAMS, basic::EmptyType)

    // define tail
    typedef basic::EmptyType tail;

    // define size
    static const size_t size = 0;
    
    // define largest type
    typedef basic::EmptyType largest_type;
};

////////////////////////////////////////////////////////////
// instantiate LSF_MAX_TEMPLATE_PARAMS GetType<> templates
//
//	template<typename T>  struct GetType<T, 0>
//	{ 
//		enum { index = 0 };			
//		typedef typename T::type1 type; 
//	};
//	
//	template<typename T>  struct GetType<T, 1>
//	{ 
//		enum { index = 1 };			
//		typedef typename T::type2 type; 
//	};
//	...
template<typename L, size_t N> 
struct GetType;

#define LSF_TYPELIST_GET(n, t)                      \
template<typename L>                                \
struct GetType<L, LSF_CNT_DEC(n)>                   \
{                                                   \
    static const size_t index = LSF_CNT_DEC(n);     \
    typedef typename L::t##n type;                  \
};

LSF_REPEAT(LSF_MAX_TEMPLATE_PARAMS, LSF_TYPELIST_GET, LSF_TYPELIST_GET, type)

#undef LSF_TYPELIST_GET

////////////////////////////////////////////////////////////
// Runtime Type Invoker
template<typename L, size_t N = 0, bool IsStop = (N >= L::size)> 
struct InvokeType
{
    template<typename F>
    typename F::return_type operator()(size_t index, F func) {
        if (index == N) {
            return func.operator()<typename GetType<L, N>::type>();
        }
        else {
            return InvokeType<L, N+1>()(index, func);
        }
    }
};

template <typename L, size_t N> 
struct InvokeType<L, N, true>
{
    template<typename F>
    typename F::return_type operator()(size_t index, F func) {
        // trigger runtime error
        throw std::runtime_error(LSF_DEBUG_INFO);
    }
};

////////////////////////////////////////////////////////////
// FindAssignableType
template<typename L, typename T, size_t N = 0, bool IsStop = (N >= L::size)>
struct FindAssignableType
{
private:
    typedef typename SelectType<
        IsAssignable<typename GetType<L, N>::type, T>::value,
        GetType<L, N>,
        FindAssignableType<L, T, N + 1> 
        >::type found;

public:
    typedef typename found::type type;

    static const size_t index = found::index;
};

template<typename L, typename T, size_t N>
struct FindAssignableType<L, T, N, true>
{
    // if not found, here will trigger a compile-time error
};

////////////////////////////////////////////////////////////
// FindType
template<typename L, typename T, size_t N = 0, bool IsStop = (N >= L::size)>
struct FindType
{
private:
    typedef typename SelectType<
        IsSame<typename GetType<L, N>::type, T>::value,
        GetType<L, N>,
        FindType<L, T, N + 1> 
        >::type found;
        
public:
    typedef typename found::type type;
    static const size_t index = found::index;
};

template<typename L, typename T, size_t N>
struct FindType<L, T, N, true>
{
    // if not found, here will trigger a compile-time error
};

} // end of namespace meta
} // end of namespace lsf

// vim:ts=4:sw=4:et:ft=cpp:
