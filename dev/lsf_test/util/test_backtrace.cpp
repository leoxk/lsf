// File:        test_backtrace.cpp
// Description: ---
// Notes:       ---
// Author:      leoxiang <leoxiang727@qq.com>
// Revision:    2015-06-15 by leoxiang

#include "lsf/basic/unit_test.hpp"
#include "lsf/util/backtrace.hpp"

using namespace std;
using namespace lsf::util;

LSF_TEST_CASE(test_backtrace) { std::cout << "backtrace:" << std::endl << LSF_BACKTRACE(2) << std::endl; }

int main(int argc, char **argv) { LSF_TEST_ALL(argc, argv); }

// vim:ts=4:sw=4:et:ft=cpp:
