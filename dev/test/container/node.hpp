// File:        node.hpp
// Description: ---
// Notes:       ---
// Author:      leoxiang <leoxiang@tencent.com>
// Revision:    2012-08-01 by leoxiang

#pragma once

#include <iostream>
#include <string>
#include "lsf/basic/type_cast.hpp"

struct TestNode {
    TestNode(uint32_t _key = 0, uint32_t _data = 0) : key(_key), data(_data) { }

    bool operator==(TestNode const & rhs) const {
        if (this->key == rhs.key && this->data == rhs.data) return true;
        else return false;
    }

    bool operator<(TestNode const & rhs) const {
        return key < rhs.key;
    }

    bool IsEmpty() { return key == 0 && data == 0; }

    std::string ToString() const {
        std::string out = "[" + lsf::basic::TypeCast< std::string>(key) + "]";
        return out;
    }

    uint32_t    key;
    uint32_t    data;
};

std::ostream & operator<<(std::ostream & os, TestNode const & node) {
    return os << "[" << node.key << "]";
}

// vim:ts=4:sw=4:et:ft=cpp:
