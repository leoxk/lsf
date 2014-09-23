// File:        variant.hpp
// Description: ---
// Notes:       ---
// Author:      leoxiang <leoxiang@tencent.com>
// Revision:    2012-11-07 by leoxiang

#pragma once

#include <stdint.h>
#include <stdexcept>
#include "lsf/basic/empty_type.hpp"
#include "lsf/meta/macro.hpp"
#include "lsf/meta/type_traits.hpp"
#include "lsf/meta/type_list.hpp"

namespace lsf {
namespace container {

////////////////////////////////////////////////////////////
// operator function define
//
// LSF_TPARAMS_DEF(5, def) expand like: 
// typename T1 = def, typename T2 = def, typename T3 = def, typename T4 = def, typename T5 = def
template<typename R, LSF_TPARAMS_DEF(LSF_MAX_TEMPLATE_PARAMS, basic::EmptyType) >
struct Functor
{
    // LSF_TYPEDEFS(5) expand like:
    // typedef T1 type1; typedef T2 type2; typedef T3 type3; typedef T4 type4; typedef T5 type5;
    LSF_TYPEDEFS(LSF_MAX_TEMPLATE_PARAMS)

    typedef R return_type;

    // here for consistency with stl
    typedef type1       argument_type;
    typedef type1       first_argument_type;
    typedef type2       second_argument_type;
    typedef return_type result_type;
};

////////////////////////////////////////////////////////////
// Variant
//
// LSF_TPARAMS_DEF(5, def) expand like: 
// typename T1 = def, typename T2 = def, typename T3 = def, typename T4 = def, typename T5 = def
template< LSF_TPARAMS_DEF(LSF_MAX_TEMPLATE_PARAMS, basic::EmptyType) >
class Variant
{
public:
    // LSF_ARGS(5, T) expand like:
    // T1, T2, T3, T4, T5
    typedef Variant< LSF_ARGS(LSF_MAX_TEMPLATE_PARAMS, T) >         this_type;
    typedef meta::TypeList< LSF_ARGS(LSF_MAX_TEMPLATE_PARAMS, T) >  list_type;
    
    // define size at compile time
    static const size_t size = sizeof(typename list_type::largest_type);

public:
    ////////////////////////////////////////////////////////////
    // contructor
    Variant() : _index(0), _base_ptr(NULL)
    { }

    Variant(this_type const & rhs) : _index(0), _base_ptr(NULL)
    {
        if (rhs.IsNull()) { _Destroy(); return; }
        // here we use invoker to trigger copy action
        meta::InvokeType<list_type>()(rhs.Index(), Copier<this_type>(*this, rhs));
    }
    
    template<typename T>
    Variant(T const & rhs) : _index(0), _base_ptr(NULL) 
    {
        // first find assignable type, see IsAssignable for more
        typedef meta::FindAssignableType<list_type, T> found;
        // then construct type obj, in store memory
        _base_ptr = (void *)(new(_store.buf) typename found::type(rhs));
        _index = found::index;
    }

    this_type & operator=(this_type const & rhs) 
    {
        if (this == &rhs) return *this;
        if (rhs.IsNull()) { _Destroy(); return *this; }
        _Destroy();
        meta::InvokeType<list_type>()(rhs.Index(), Copier<this_type>(*this, rhs));
        return *this;
    }

    template<typename T>
    this_type & operator=(T const & rhs) 
    {
        typedef meta::FindAssignableType<list_type, T> found;
        _Destroy();
        _base_ptr = (void *)(new(_store.buf) typename found::type(rhs));
        _index = found::index;
        return *this;
    }

    ~Variant() { _Destroy(); }

    //////////////////////////////////////////////////////////// 
    // apply visitor func
    template<typename F>
    typename F::return_type ApplyVisit(F func) {
        if (IsNull()) throw std::runtime_error(LSF_DEBUG_INFO);

        Visitor<this_type, F> visitor(*this, func);
        return meta::InvokeType<list_type>()(_index, visitor);
    }

    //////////////////////////////////////////////////////////// 
    // get func
    template<typename T>
    T & Get()
    {
        typedef meta::FindType<list_type, T> found;
        if (_index != found::index) throw std::runtime_error(LSF_DEBUG_INFO);
        return *(static_cast<T *>(_base_ptr));
    }

    template<typename T>
    T const & Get() const
    {
        typedef meta::FindType<list_type, T> found;
        if (_index != found::index) throw std::runtime_error(LSF_DEBUG_INFO);
        return *(static_cast<T const *>(_base_ptr));
    }

    template<typename T>
    T * GetPtr()
    {
        typedef meta::FindType<list_type, T> found;
        if (_index != found::index) return NULL;
        return static_cast<T *>(_base_ptr);
    }

    template<typename T>
    T const * GetPtr() const
    {
        typedef meta::FindType<list_type, T> found;
        if (_index != found::index) return NULL;
        return static_cast<T const *>(_base_ptr);
    }

    //////////////////////////////////////////////////////////// 
    // access func
    size_t Index() const { return _index; }
    bool  IsNull() const { return _base_ptr == NULL; }

private:
    void _Destroy() {
        if (IsNull()) return;
        meta::InvokeType<list_type>()(_index, Deleter(_base_ptr));
        _base_ptr = NULL;
    }

    struct Deleter : public Functor<void>
    {
        Deleter(void * _base_ptr) : base_ptr(_base_ptr) { }
        
        template<typename T>
        void operator()() { 
            static_cast<T *>(base_ptr)->~T(); 
        }

        void * base_ptr;
    };

    template<typename V>	
    struct Copier : public Functor<void> 
    {
        Copier(V & _lhs, V const & _rhs) : lhs(_lhs), rhs(_rhs) { }

        template<typename T>
        void operator()() {
            lhs._base_ptr = new(lhs._store.buf) T(rhs.Get<T>());
            lhs._index = rhs._index;
        };

        V       &   lhs;
        V const &   rhs;
    };

    template<typename V, typename F>	
    struct Visitor : public Functor<typename F::return_type> 
    {
        Visitor(V & _lhs, F _func) : lhs(_lhs), func(_func) { }

        template<typename T>
        typename F::return_type operator()() {
            return func(lhs.Get<T>());
        };

        V & lhs;
        F   func;
    };

private:
    template<int N> 
    struct Storage { uint8_t buf[N]; };

    size_t _index;
    void * _base_ptr;
    Storage<sizeof(typename list_type::largest_type)> _store;
};

} // end of namespace container
} // end of namespace lsf

// vim:ts=4:sw=4:et:ft=cpp:
