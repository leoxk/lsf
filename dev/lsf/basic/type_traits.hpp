// File:        type_traits.hpp
// Description: ---
// Notes:       ---
// Author:      leoxiang <leoxiang727@qq.com>
// Revision:    2015-07-02 by leoxiang

#pragma once

#include <type_traits>
#include <cstddef>
#include <tuple>
#include "lsf/basic/macro.hpp"
#include "lsf/basic/empty_type.hpp"

namespace lsf {
namespace basic {

////////////////////////////////////////////////////////////
// integer_max
template<size_t Arg, size_t... Rest>
struct integer_max;

template<size_t Arg>
struct integer_max<Arg> : std::integral_constant<size_t, Arg>
{};

template<size_t arg1, size_t arg2, size_t... Rest>
struct integer_max<arg1, arg2, Rest...> : std::integral_constant<size_t,
    arg1 >= arg2 ?
    integer_max<arg1, Rest...>::value :
    integer_max<arg2, Rest...>::value>
{};

////////////////////////////////////////////////////////////
// contain
template<typename T, typename... List>
struct contain : std::true_type
{};

template<typename T, typename Head, typename... Rest>
struct contain<T, Head, Rest...>
    : std::conditional<std::is_same<T, Head>::value, std::true_type, contain<T, Rest...>>::type
{};

template<typename T>
struct contain<T> : std::false_type
{};

////////////////////////////////////////////////////////////
// index
template<typename T, typename... Ts>
struct index;

template<typename T, typename First, typename... Ts>
struct index<T, First, Ts...> : std::integral_constant<size_t, index<T, Ts...>::value + 1>
{};

template<typename T, typename... Ts>
struct index<T, T, Ts...> : index<T>
{};

template<typename T>
struct index<T> : std::integral_constant<size_t, 0>
{};

////////////////////////////////////////////////////////////
// index_type
template<int N, typename... Ts>
struct index_type {
    static_assert(N < sizeof...(Ts), LSF_DEBUG_INFO);
    using type = typename std::tuple_element<N, std::tuple<Ts...>>::type;
};

////////////////////////////////////////////////////////////
// is_assignable
template<typename T1, typename T2>
struct is_assignable : std::conditional<std::is_same<typename std::decay<T1>::type, typename std::decay<T2>::type>::value,
    std::true_type, std::false_type>::type
{};

// const pointer
template<typename T>
struct is_assignable<T const*, T*> : std::true_type {};

// double float convert
template<>
struct is_assignable<double, float> : std::true_type {};

template<>
struct is_assignable<float, double> : std::true_type {};

////////////////////////////////////////////////////////////
// find_assignable
template<typename T, typename... Ts>
struct find_assignable;

template<typename T, typename T1, typename... Ts>
struct find_assignable<T, T1, Ts...> {
    using type = typename std::conditional<is_assignable<T1, T>::value, T1, typename find_assignable<T, Ts...>::type>::type;
};

template<typename T>
struct find_assignable<T> {
    using type = void;
};

////////////////////////////////////////////////////////////
// has_assignable
// template<typename T, typename... Ts>
// struct find_assignable;
//
// template<typename T, typename T1, typename... Ts>
// struct find_assignable<T, T1, Ts...> : std::conditional<is_assignable<T, T1>::value, T1, find_assignable<T, Ts...>>
// {};
//
// template<typename T>
// struct find_assignable<T> : std::conditional<true, EmptyType, EmptyType>
// {};
//
////////////////////////////////////////////////////////////
// function_traits
template<class F>
struct function_traits;

// function pointer
template<class R, class... Args>
struct function_traits<R(*)(Args...)> : function_traits<R(Args...)>
{};

// member function pointer
template<class C, class R, class... Args>
struct function_traits<R(C::*)(Args...)> : function_traits<R(C&,Args...)>
{};

// const member function pointer
template<class C, class R, class... Args>
struct function_traits<R(C::*)(Args...) const> : function_traits<R(C&,Args...)>
{};

// member object pointer
template<class C, class R>
struct function_traits<R(C::*)> : function_traits<R(C&)>
{};

// functor
template<class F>
struct function_traits : function_traits<decltype(&std::decay<F>::type::operator())>
{};

// raw function type
template<class R, class... Args>
struct function_traits<R(Args...)> {

    static constexpr std::size_t argument_size = sizeof...(Args);

    using return_type = R;

    template <size_t N>
    struct argument {
        using type = typename index_type<N, Args...>::type;
    };
};

} // end namespace basic
} // end namespace lsf

// vim:ts=4:sw=4:et:ft=cpp:
