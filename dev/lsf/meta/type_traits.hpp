// File:        type_traits.hpp
// Description: ---
// Notes:       ---
// Author:      leoxiang <leoxiang@tencent.com>
// Revision:    2012-11-08 by leoxiang

#pragma once

#include <cstddef>

namespace lsf { 
namespace meta {

////////////////////////////////////////////////////////////
// ConstantType
template<typename T, T VAL>
struct ConstantType
{
    typedef T value_type;
    static const T value = VAL;
};

typedef ConstantType<bool, true>    TrueType;
typedef ConstantType<bool, false>   FalseType;

////////////////////////////////////////////////////////////
// SelectType
template<bool Cond, typename T1, typename T2>
struct SelectType { typedef T1 type; };

template <typename T1, typename T2>
struct SelectType<false, T1, T2> { typedef T2 type; };

////////////////////////////////////////////////////////////
// RemoveCV
template<typename T>
struct RemoveCV { typedef T type; };

template<typename T>
struct RemoveCV<const T> { typedef T type; };

template<typename T>
struct RemoveCV<volatile T> { typedef T type; };

template<typename T>
struct RemoveCV<const volatile T> { typedef T type; };

template<typename T>
struct RemoveCV<const T *> { typedef T * type; };

template<typename T>
struct RemoveCV<volatile T *> { typedef T * type; };

template<typename T>
struct RemoveCV<const volatile T *> { typedef T * type; };

template<typename T>
struct RemoveCV<const T &> { typedef T & type; };

template<typename T>
struct RemoveCV<volatile T &> { typedef T & type; };

template<typename T>
struct RemoveCV<const volatile T &> { typedef T & type; };

////////////////////////////////////////////////////////////
// IsSame
template<typename T1, typename T2>
struct IsSame : public FalseType { };

template<typename T>
struct IsSame<T, T> : public TrueType { };

////////////////////////////////////////////////////////////
// IsConst
template<typename T>
struct IsConst : public FalseType { };

template<typename T>
struct IsConst<const T> : public TrueType { };

template<typename T>
struct IsConst<T const &> : public TrueType { };

template<typename T>
struct IsConst<T const *> : public TrueType { };

////////////////////////////////////////////////////////////
// IsReference
template<typename T>
struct IsReference : public FalseType { };

template<typename T>
struct IsReference<T &> : public TrueType { };

template<typename T>
struct IsConstReference : public FalseType { };

template<typename T>
struct IsConstReference<T const &> : public TrueType { };

////////////////////////////////////////////////////////////
// IsPointer
template<typename T>
struct IsPointer : public FalseType { };

template<typename T>
struct IsPointer<T *> : public TrueType { };

template<typename T>
struct IsConstPointer : public FalseType { };

template<typename T>
struct IsConstPointer<T const *> : public TrueType { };

////////////////////////////////////////////////////////////
// CallTraits
template <typename T>
struct CallTrait
{
public:
   typedef T            value_type;
   typedef T &          reference;
   typedef T const &    const_reference;

   typedef T const &    param_type;
   typedef T &          return_type;
   typedef T const &    const_return_type;
};

template <typename T>
struct CallTrait<T &>
{
   typedef T &          value_type;
   typedef T &          reference;
   typedef T const &    const_reference;

   typedef T &          param_type;
   typedef T &          return_type;
   typedef T const &    const_return_type;
};

template <typename T>
struct CallTrait<T const &>
{
   typedef T const &    value_type;
   typedef T &          reference;
   typedef T const &    const_reference;

   typedef T const &    param_type;
   typedef T const &    return_type;
   typedef T const &    const_return_type;
};

////////////////////////////////////////////////////////////
// IsAssignable
template<typename T1, typename T2>
struct IsAssignable : public FalseType { };		

// equal type
template<typename T>
struct IsAssignable<T, T> : public TrueType { };		

template<typename T> 
struct IsAssignable<const T, T> : public TrueType { }; 

template<typename T> 
struct IsAssignable<T, const T> : public TrueType { }; 

// refer
template<typename T> 
struct IsAssignable<T, T &> : public TrueType { }; 

template<typename T> 
struct IsAssignable<T, T const &> : public TrueType { }; 

template<typename T> 
struct IsAssignable<T &, T> : public TrueType { }; 

template<typename T> 
struct IsAssignable<T const &, T> : public FalseType { }; 

template<typename T> 
struct IsAssignable<T const &, T const > : public FalseType { }; 

template<typename T> 
struct IsAssignable<T const &, T const &> : public FalseType { }; 

// pointer
template<typename T, size_t N>
struct IsAssignable<T *, T[N]> : public TrueType { };

template<typename T> 
struct IsAssignable<T const *, T *> : public TrueType { };

template<typename T, size_t N>
struct IsAssignable<T const *, T[N]> : public TrueType { };

// double float convert
template<>
struct IsAssignable<float, double> : public TrueType { };

template<>
struct IsAssignable<double, float> : public TrueType { };

} // end of namespace meta
} // end of namespace lsf

// vim:ts=4:sw=4:et:ft=cpp:
