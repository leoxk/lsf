// File:        test_combination.cpp
// Description: ---
// Notes:       ---
// Author:      leoxiang <xiangkun@ximigame.com>
// Revision:    2014-09-26 by leoxiang
//
// Copyright (C) 1998-2014 XiMi. All Rights Reserved.
// This is unpublished proprietary source code of XiMi Ltd. The copyright
// notice above does not evidence any actual or intended publication of such
// source code. UNAUTHORIZED DISTRIBUTION, ADAPTATION OR USE MAY BE SUBJECT
// TO CIVIL AND CRIMINAL PENALTIES.

#include "lsf/basic/unit_test.hpp"
#include "lsf/algorithm/combination.hpp"
#include "lsf/basic/type_cast.hpp"

using namespace std;
using namespace lsf::algorithm;
using namespace lsf::basic;

typedef vector<vector<vector<int>::iterator>> result_type;
LSF_TEST_CASE(test_combination)
{
    vector<int> vec;
    vector<vector<vector<int>::iterator>> combin_result;

    vec.push_back(1);
    vec.push_back(2);
    vec.push_back(3);
    vec.push_back(4);
    vec.push_back(5);

    Combination::FindCombination(vec.begin(), vec.end(), 3, combin_result);

    vector<string> result_vec;
    for (result_type::iterator it = combin_result.begin(); it != combin_result.end(); ++it)
    {
        string result;
        for (result_type::value_type::iterator iter_val = it->begin(); iter_val != it->end(); ++iter_val)
        {
            result += TypeCast<string>(**iter_val);
            if ((iter_val + 1) != it->end()) result += ", ";
        }
        result_vec.push_back(result);
    }

    LSF_ASSERT(result_vec[0] == "1, 2, 3");
    LSF_ASSERT(result_vec[1] == "1, 2, 4");
    LSF_ASSERT(result_vec[2] == "1, 3, 4");
    LSF_ASSERT(result_vec[3] == "2, 3, 4");
    LSF_ASSERT(result_vec[4] == "1, 2, 5");
    LSF_ASSERT(result_vec[5] == "1, 3, 5");
    LSF_ASSERT(result_vec[6] == "2, 3, 5");
    LSF_ASSERT(result_vec[7] == "1, 4, 5");
    LSF_ASSERT(result_vec[8] == "2, 4, 5");
    LSF_ASSERT(result_vec[9] == "3, 4, 5");
}

int main(int argc, char **argv)
{
	LSF_TEST_ALL();
}

// vim:ts=4:sw=4:et:ft=cpp:
