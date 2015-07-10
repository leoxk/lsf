// File:        any.hpp
// Description: ---
// Notes:       ---
// Author:      leoxiang <leoxiang727@qq.com>
// Revision:    2015-07-01 by leoxiang

#pragma once

#include <iostream>
#include <string>
#include <memory>
#include <typeindex>

namespace lsf {
namespace basic {

class Any {
public:
    // constructor
    Any() : _type_index(typeid(void)) {}

    Any(Any const& rhs) : _ptr(rhs._ptr->Clone()), _type_index(rhs._type_index) {}

    Any(Any&& rhs) : _ptr(std::move(rhs._ptr)), _type_index(rhs._type_index) {}

    template<typename RealType,
        class = typename std::enable_if<!std::is_same<typename std::decay<RealType>::type, Any>::value, RealType>::type>
    Any(RealType&& value) :
        _ptr(new Derived<typename std::decay<RealType>::type>(std::forward<RealType>(value))),
        _type_index(typeid(typename std::decay<RealType>::type)) {}

    // copy assignment
    Any& operator=(Any const& rhs) {
        if (_ptr == rhs._ptr) return *this;
        _ptr = rhs._ptr->Clone();
        _type_index = rhs._type_index;
        return *this;
    }

    Any& operator=(Any&& rhs) {
        if (_ptr == rhs._ptr) return *this;
        _ptr = rhs._ptr->Transfer();
        _type_index = rhs._type_index;
        return *this;
    }

    // common func
    explicit operator bool() const { return !_ptr; }

    template<class RealType>
    bool Is() const { return _type_index == std::type_index(typeid(typename std::decay<RealType>::type)); }

    template<class RealType>
    typename std::decay<RealType>::type& Cast() {
        if (!Is<RealType>()) throw std::bad_cast();
        auto derived = dynamic_cast<Derived<typename std::decay<RealType>::type>*>(_ptr.get());
        return derived->value;
    }

    template<class RealType>
    typename std::decay<RealType>::type const& Cast() const {
        if (!Is<RealType>()) throw std::bad_cast();
        auto derived = dynamic_cast<Derived<typename std::decay<RealType>::type>*>(_ptr.get());
        return derived->value;
    }

private:
    struct Base {
        virtual ~Base() {}
        virtual std::unique_ptr<Base> Clone() const = 0;
        virtual std::unique_ptr<Base> Transfer() const = 0;
    };

    template<typename RealType>
    struct Derived : public Base {
        template<typename OtherType>
        Derived(OtherType&& value) : value(std::forward<OtherType>(value)) { }
        std::unique_ptr<Base> Clone()    const { return std::unique_ptr<Base>(new Derived<RealType>(value)); }
        std::unique_ptr<Base> Transfer() const { return std::unique_ptr<Base>(new Derived<RealType>(std::move(value))); }
        RealType value;
    };

private:
    std::unique_ptr<Base> _ptr;
    std::type_index _type_index;
};

} // end namespace basic
} // end namespace lsf

// vim:ts=4:sw=4:et:ft=cpp:
