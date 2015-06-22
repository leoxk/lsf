// File:        node.hpp
// Description: ---
// Notes:       ---
// Author:      leoxiang <leoxiang727@qq.com>
// Revision:    2012-08-01 by leoxiang

#pragma once

#include <iostream>
#include <string>
#include "lsf/util/type_cast.hpp"

class TestNode {
public:
    TestNode(uint32_t _key = 0, uint32_t _data = 0) : key(_key), data(_data) {}

    bool operator==(TestNode const& rhs) const {
        if (this->key == rhs.key && this->data == rhs.data)
            return true;
        else
            return false;
    }

    bool operator<(TestNode const& rhs) const { return key < rhs.key; }

    bool empty() { return key == 0 && data == 0; }

    std::string ToString() const {
        std::string out = "[" + lsf::util::TypeCast<std::string>(key) + "]";
        return out;
    }

    uint32_t key;
    uint32_t data;
};

class TestNode1 : public TestNode {
public:
    uint32_t other;
};

std::ostream& operator<<(std::ostream& os, TestNode const& node) { return os << "[" << node.key << "]"; }

// vim:ts=4:sw=4:et:ft=cpp:
