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

template<typename IterType>
class Combination
{
public: 
    typedef IterType                iter_type;
    typedef std::vector<IterType>   iter_vec_type;
    typedef Combination<IterType>   base_type;

public:
    virtual bool OnGetCombination(iter_vec_type iter_vec, size_t m_count, size_t n_count) = 0;

    bool FindCombination(iter_type it_begin, iter_type it_end, size_t n_count)
    {
        size_t m_count = it_end - it_begin;

        if (m_count < n_count) return false;

        // init 01 array
        int8_t * vec = new int8_t[m_count];
        ::memset(vec, 0, sizeof(int8_t) * m_count);

        // the first is also a combination
        for (size_t i = 0; i < n_count; i++) vec[i] = 1;
        if (!_GenCombination(it_begin, it_end, vec, m_count, n_count)) return false;

        // get all combination with 01 algorithm
        for (size_t i = 0; i < m_count-1; )
        {
            if (vec[i] == 1 && vec[i+1] == 0)
            {
                // swap
                vec[i] = 0;
                vec[i+1] = 1;

                // move all 1 to the left
                std::sort(vec, vec+i, std::greater<int>());

                // get a new combination
                if (!_GenCombination(it_begin, it_end, vec, m_count, n_count)) return false;
                
                // repeat
                i = 0;
                continue;
            }
            i++;
        }

        // beware of free memory
        delete [] vec;
        return true;
    }

private:
    bool _GenCombination(iter_type it_being, iter_type it_end, int8_t * vec, size_t m_count, size_t n_count)
    {
        iter_vec_type iter_vec;

        size_t i = 0;
        for (iter_type it = it_being; it != it_end;)
        {
            if (vec[i] == 1) iter_vec.push_back(it);
            ++it;
            ++i;
        }
        
        return OnGetCombination(iter_vec, m_count, n_count);
    }
};

} // end of namespace algorithm
} // end of namespace lsf


// vim:ts=4:sw=4:et:ft=cpp:
