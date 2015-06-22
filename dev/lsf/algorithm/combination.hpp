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

namespace lsf {
namespace algorithm {

namespace detail {

bool next_combination(int8_t* vec, size_t count) {
    for (size_t i = 0; i < count - 1; ++i) {
        if (vec[i] == 1 && vec[i + 1] == 0) {
            // swap
            std::swap(vec[i], vec[i + 1]);

            // move all 1 to the left
            std::sort(vec, vec + i, std::greater<int>());

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

    // init 01 array
    int8_t* vec = new int8_t[count]{};
    std::fill_n(vec, cmb_count, 1);

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

    // free memory
    delete[] vec;
    return true;
}

}  // end of namespace algorithm
}  // end of namespace lsf

// vim:ts=4:sw=4:et:ft=cpp:
