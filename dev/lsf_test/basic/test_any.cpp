// File:        test_any.cpp
// Description: ---
// Notes:       ---
// Author:      leoxiang <leoxiang727@qq.com>
// Revision:    2015-07-01 by leoxiang

#include <vector>
#include "lsf/basic/unit_test.hpp"
#include "lsf/basic/any.hpp"

using namespace std;
using namespace lsf::basic;

LSF_TEST_CASE(any) {
    // test assign
    Any i = int(10);
    LSF_ASSERT(i.Is<int>());
    LSF_ASSERT(i.Is<int &>()); // reference ok
    LSF_ASSERT(i.Is<int const>()); // const ok
    LSF_ASSERT(i.Is<int volatile>()); // volatile ok
    LSF_ASSERT(i.Is<int const &>()); // volatile ok

    LSF_ASSERT(i.Cast<int>() == 10);
    LSF_ASSERT(i.Cast<int &>() == 10);
    LSF_ASSERT(i.Cast<int const>() == 10);
    LSF_ASSERT(i.Cast<int const &>() == 10);

    // test exception when bad cast
    try { i.Cast<char *>(); } catch (std::exception & e) {
        LSF_ASSERT(typeid(e) == typeid(std::bad_cast));
    }

    // test copy
    std::vector<int> vec = { 1, 2, 3 };
    Any any_v = vec;
    vec.clear();
    LSF_ASSERT(any_v.Cast<decltype(vec)>().size() == 3);

    // test pointer
    int * pint = new int(100);
    Any any_p = pint;
    LSF_ASSERT(any_p.Cast<int*>() == pint);
    *pint = 0;
    LSF_ASSERT(*any_p.Cast<int*>() == 0);

    // test destructor
    std::shared_ptr<int> spint(new int(10));
    Any any_sp = spint;
    LSF_ASSERT(spint.use_count() == 2);
    any_sp = int(10);
    LSF_ASSERT(spint.use_count() == 1);

    // test move
    any_sp = std::move(spint);
    LSF_ASSERT(any_sp.Cast<decltype(spint)>().use_count() == 1);

    // test const
    Any any_i = 100;
    Any const & r_any_i = any_i;
    LSF_ASSERT(r_any_i.Is<int>());
    LSF_ASSERT(r_any_i.Cast<int>() == 100);
}


LSF_TEST_CASE(default_type) {
    // test default literal
    Any i;
    i = 100; LSF_ASSERT(i.Is<int>());
    i = "123"; LSF_ASSERT(i.Is<char const *>());
    i = -1; LSF_ASSERT(i.Is<int>());
    i = 01; LSF_ASSERT(i.Is<int>());
    i = 1.1; LSF_ASSERT(i.Is<double>());
    i = -1.1; LSF_ASSERT(i.Is<double>());
}


int f1(int a) { return 0;}
int f2(int a,int b) { return 0;}
int f3(int a,int b,int c) { return 0;}

LSF_TEST_CASE(test_func) {
    using F1 = int(int);
    using F2 = int(int,int);
    using F3 = int(int,int,int);

    Any f;
    f =  f1; LSF_ASSERT(f.Is<F1>()); LSF_ASSERT(f.Cast<F1>()(1) == 0);
    f =  f2; LSF_ASSERT(f.Is<F2>()); LSF_ASSERT(f.Cast<F2>()(1,1) == 0);
    f =  f3; LSF_ASSERT(f.Is<F3>()); LSF_ASSERT(f.Cast<F3>()(1,1,1) == 0);
    f = &f1; LSF_ASSERT(f.Is<F1>()); LSF_ASSERT(f.Cast<F1>()(1) == 0);
    f = &f2; LSF_ASSERT(f.Is<F2>()); LSF_ASSERT(f.Cast<F2>()(1,1) == 0);
    f = &f3; LSF_ASSERT(f.Is<F3>()); LSF_ASSERT(f.Cast<F3>()(1,1,1) == 0);

    try { f.Cast<F1>(); } catch(std::exception& e) {
        LSF_ASSERT(typeid(e) == typeid(std::bad_cast));
    }
}

struct A {
int mf1(int a) { return 0;}
int mf2(int a,int b) { return 0;}
int mf3(int a,int b,int c) { return 0;}
};

LSF_TEST_CASE(test_member_func) {
    using MF1 = int(A::*)(int);
    using MF2 = int(A::*)(int,int);
    using MF3 = int(A::*)(int,int,int);

    Any mf;
    A a;
    mf = &A::mf1; LSF_ASSERT(mf.Is<MF1>()); LSF_ASSERT((a.*mf.Cast<MF1>())(1) == 0);
    mf = &A::mf2; LSF_ASSERT(mf.Is<MF2>()); LSF_ASSERT((a.*mf.Cast<MF2>())(1,1) == 0);
    mf = &A::mf3; LSF_ASSERT(mf.Is<MF3>()); LSF_ASSERT((a.*mf.Cast<MF3>())(1,1,1) == 0);

    try { mf.Cast<MF1>(); } catch(std::exception& e) {
        LSF_ASSERT(typeid(e) == typeid(std::bad_cast));
    }
}

int main(int argc, char** argv) { LSF_TEST_ALL(argc, argv); }


// vim:ts=4:sw=4:et:ft=cpp:
