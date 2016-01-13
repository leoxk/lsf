// File:        combination.hpp
// Description: ---
// Notes:       ---
// Author:      leoxiang <leoxiang727@qq.com>
// Revision:    2014-09-26 by leoxiang

#pragma once
#include <cstring>
#include <vector>
#include <algorithm>
#include "lsf/basic/macro.hpp"
#include "lsf/basic/scope_exit.hpp"

namespace lsf {
namespace algorithm {

namespace detail {

bool next_combination(std::vector<int8_t> & vec, size_t count) {
    for (size_t i = 0; i < count - 1; ++i) {
        if (vec[i] == 1 && vec[i + 1] == 0) {
            // swap
            std::swap(vec[i], vec[i + 1]);

            // move all 1 to the left
            std::sort(vec.begin(), vec.begin() + i, std::greater<int8_t>());

            return true;
        }
    }

    return false;
}

}  // end of namespace detail

template <typename IterType, typename FuncType>
bool for_combination(IterType begin, IterType end, size_t cmb_count, FuncType func) {
    std::vector<IterType> combination;
    size_t count = end - begin;

    // check input
    if (count < cmb_count) return false;

    // alloc and init array
    std::vector<int8_t> vec;
    vec.reserve(count);
    vec.insert(vec.end(), cmb_count, 1);
    vec.insert(vec.end(), count - cmb_count, 0);

    // traverse all combination
    do {
        combination.clear();

        size_t i = 0;
        IterType iter = begin;
        while (i < count) {
            if (vec[i]) combination.push_back(iter);
            ++iter;
            ++i;
        }

        func(combination);
    } while (detail::next_combination(vec, count));

    return true;
}

}  // end of namespace algorithm
}  // end of namespace lsf

// vim:ts=4:sw=4:et:ft=cpp:
