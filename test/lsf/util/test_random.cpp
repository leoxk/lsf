// File:        test_random.cpp
// Description: ---
// Notes:       ---
// Author:      leoxiang <leoxiang727@qq.com>
// Revision:    2012-08-09 by leoxiang

#include <iostream>
#include "lsf/basic/unit_test.hpp"
#include "lsf/util/random.hpp"

using namespace std;
using namespace lsf::util;

LSF_TEST_CASE(test)
{
    // batch test
    for (int i = 0; i < 1000; i++) {
        size_t rand = SingleRandom::GetRand(0, 100);
        LSF_ASSERT_ERR_ONLY(rand >= 0 && rand <= 100);
    }

    // test special var
    LSF_ASSERT(SingleRandom::GetRand(0) == 0);
    LSF_ASSERT(SingleRandom::GetRand(1, 1) == 1);
    LSF_ASSERT(SingleRandom::GetRand(1, 0) == 1);
}

int main(int argc, char **argv)
{
	LSF_TEST_ALL();
}

// vim:ts=4:sw=4:et:ft=cpp:
