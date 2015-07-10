// File:        test_scope_exit.cpp
// Description: ---
// Notes:       ---
// Author:      leoxiang <leoxiang727@qq.com>
// Revision:    2015-06-30 by leoxiang

#include "lsf/basic/unit_test.hpp"
#include "lsf/basic/scope_exit.hpp"

using namespace std;
using namespace lsf::basic;

void func(int * & pint) {
    delete pint;
    pint = nullptr;
}

LSF_TEST_CASE(scope_exit) {

    int * pint = new int;

    { LSF_SCOPE_EXIT() { delete pint; pint = nullptr; }; }
    LSF_ASSERT(pint == nullptr);

    pint = new int;
    { auto scope_exit = detail::ScopeExitCreator() << [&] { delete pint; pint = nullptr; }; }
    LSF_ASSERT(pint == nullptr);

    pint = new int;
    {
        auto lambda = [&] { delete pint; pint = nullptr; };
        auto scope_exit = detail::ScopeExitCreator() << lambda;
    }
    LSF_ASSERT(pint == nullptr);

    pint = new int;
    {
        auto scope_exit = detail::ScopeExitCreator() << std::bind(func, std::ref(pint));
    }
    LSF_ASSERT(pint == nullptr);
}

LSF_TEST_CASE(scope_exit_in_loop) {

    int array;
    for (auto i : {0,1,2,3,4,5,6,7,8,9}) {
        // assert
        if (i != 0) LSF_ASSERT(array == (i-1));

        // scope logic
        LSF_SCOPE_EXIT() { array = i; };
    }
}

int main(int argc, char** argv) { LSF_TEST_ALL(argc, argv); }

// vim:ts=4:sw=4:et:ft=cpp:
