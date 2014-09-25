// File:        singleton.hpp
// Description: ---
// Notes:       ---
// Author:      leoxiang <leoxiang@tencent.com>
// Revision:    2011-12-26 by leoxiang

#pragma once

namespace lsf {
namespace basic {

template<typename ObjectType>
class Singleton
{
public:
    static ObjectType * Instance() {
        return &Reference();
    }

    static ObjectType & Reference() {
        static ObjectType _Instance;
        return _Instance;
    }
 
protected:
    Singleton() { }                     // construtor is hidden
    Singleton(Singleton const &) { }    // copy constructor is hidden
};

} // end of namespace basic
} // end of namespace lsf

// vim:ts=4:sw=4:et:ft=cpp: