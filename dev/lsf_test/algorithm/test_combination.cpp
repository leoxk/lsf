// File:        test_combination.cpp
// Description: ---
// Notes:       ---
// Author:      leoxiang <leoxiang727@qq.com>
// Revision:    2014-09-26 by leoxiang

#include "lsf/basic/unit_test.hpp"
#include "lsf/algorithm/combination.hpp"
#include "lsf/basic/type_cast.hpp"

using namespace std;
using namespace lsf::algorithm;
using namespace lsf::basic;

class TestCombination : public lsf::algorithm::Combination<vector<int>::iterator>
{
public:
    bool OnGetCombination(iter_vec_type iter_vec, size_t m_count, size_t n_count)
    {
        string result;
        for (iter_vec_type::iterator it = iter_vec.begin(); it != iter_vec.end(); ++it)
        {
            result += TypeCast<string>(**it);
            if ((it + 1) != iter_vec.end()) result += ", ";
        }
        result_vec.push_back(result);
        return true;
    }

public:
    vector<string> result_vec;
};

typedef vector<vector<vector<int>::iterator>> result_type;
LSF_TEST_CASE(test_combination)
{
    vector<int> vec;
    vec.push_back(1);
    vec.push_back(2);
    vec.push_back(3);
    vec.push_back(4);
    vec.push_back(5);

    TestCombination test;
    test.FindCombination(vec.begin(), vec.end(), 3);

    LSF_ASSERT(test.result_vec.size() == 10);
    LSF_ASSERT(test.result_vec[0] == "1, 2, 3");
    LSF_ASSERT(test.result_vec[1] == "1, 2, 4");
    LSF_ASSERT(test.result_vec[2] == "1, 3, 4");
    LSF_ASSERT(test.result_vec[3] == "2, 3, 4");
    LSF_ASSERT(test.result_vec[4] == "1, 2, 5");
    LSF_ASSERT(test.result_vec[5] == "1, 3, 5");
    LSF_ASSERT(test.result_vec[6] == "2, 3, 5");
    LSF_ASSERT(test.result_vec[7] == "1, 4, 5");
    LSF_ASSERT(test.result_vec[8] == "2, 4, 5");
    LSF_ASSERT(test.result_vec[9] == "3, 4, 5");
}

int main(int argc, char **argv)
{
	LSF_TEST_ALL();
}

// vim:ts=4:sw=4:et:ft=cpp:
