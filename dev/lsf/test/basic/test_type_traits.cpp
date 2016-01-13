// File:        test_type_traits.cpp
// Description: ---
// Notes:       ---
// Author:      leoxiang <leoxiang727@qq.com>
// Revision:    2015-07-02 by leoxiang

#include "lsf/basic/unit_test.hpp"
#include "lsf/basic/type_traits.hpp"

using namespace lsf::basic;

LSF_TEST_CASE(test_meta) {
    // max
    static_assert(integer_max<1,2,3,4,5>::value == 5, "");

    // contain
    static_assert(contain<int, size_t, uint32_t, bool, char, int>::value, "");
    static_assert(!contain<int, size_t, uint32_t, bool, char>::value, "");

    // get index
    static_assert(lsf::basic::index<int, size_t, uint32_t, bool, char, int>::value == 4, "");
    static_assert(lsf::basic::index<int, size_t, uint32_t, bool, int, char, char>::value == 3, "");
    static_assert(lsf::basic::index<int, int>::value == 0, "");
    static_assert(lsf::basic::index<int>::value == 0, "");

    // get type
    static_assert(std::is_same<index_type<4, size_t, uint32_t, bool, char, int>::type, int>::value, "");
    static_assert(std::is_same<index_type<0, int, size_t, uint32_t, bool, char, int>::type, int>::value, "");
}

LSF_TEST_CASE(test_assignable) {
    // test is_assignable
    static_assert(is_assignable<int, int>::value, "");
    static_assert(is_assignable<int, int const>::value, "");
    static_assert(is_assignable<int, int&>::value, "");
    static_assert(is_assignable<int, int const&>::value, "");
    static_assert(is_assignable<int&, int>::value, "");
    static_assert(is_assignable<int&, int&>::value, "");
    static_assert(is_assignable<int const&, int>::value, "");
    static_assert(is_assignable<int const&, int const>::value, "");
    static_assert(is_assignable<int const&, int&>::value, "");
    static_assert(is_assignable<int const&, int const&>::value, "");
    static_assert(is_assignable<int const*, int*>::value, "");
    static_assert(is_assignable<double, float>::value, "");
    static_assert(is_assignable<char const*, char const[100]>::value, "");

    // test find_assignable
    static_assert(std::is_same<find_assignable<int, int, char *>::type, int>::value, "");
    static_assert(std::is_same<find_assignable<int, char, int&, char *>::type, int&>::value, "");
    static_assert(std::is_same<find_assignable<float, int, double, char *>::type, double>::value, "");
    static_assert(std::is_same<find_assignable<int, float, char*>::type, void>::value, "");
    static_assert(std::is_same<find_assignable<int&, float, char*, int>::type, int>::value, "");
    static_assert(std::is_same<find_assignable<int&, float, char*, int&>::type, int&>::value, "");
    static_assert(std::is_same<find_assignable<char const[7], int, float, char, char const*>::type, char const*>::value, "");
}

LSF_TEST_CASE(test_function_traits) {
    // normal function
    static_assert(function_traits<int(int, int&, char const*)>::argument_size == 3, "");
    static_assert(std::is_same<function_traits<int(int, int&, char const*)>::argument<0>::type, int>::value, "");
    static_assert(std::is_same<function_traits<int(int, int&, char const*)>::argument<1>::type, int&>::value, "");
    static_assert(std::is_same<function_traits<int(int, int&, char const*)>::argument<2>::type, char const*>::value, "");
    static_assert(std::is_same<function_traits<int(int, int&, char const*)>::return_type, int>::value, "");
    static_assert(std::is_same<function_traits<void(void)>::return_type, void>::value, "");
    static_assert(std::is_same<function_traits<void(int)>::argument<0>::type, int>::value, "");

    // member function
    struct A {
        int dummy1(int, char *, void const *);
        int dummy2(float, char *, void const *) const;
    };
    static_assert(function_traits<decltype(&A::dummy1)>::argument_size == 4, "");
    static_assert(function_traits<decltype(&A::dummy2)>::argument_size == 4, "");
    static_assert(std::is_same<function_traits<decltype(&A::dummy1)>::argument<0>::type, A&>::value, "");
    static_assert(std::is_same<function_traits<decltype(&A::dummy1)>::argument<1>::type, int>::value, "");
    static_assert(std::is_same<function_traits<decltype(&A::dummy2)>::argument<0>::type, A&>::value, "");
    static_assert(std::is_same<function_traits<decltype(&A::dummy2)>::argument<1>::type, float>::value, "");
    static_assert(std::is_same<function_traits<decltype(&A::dummy2)>::argument<2>::type, char*>::value, "");
    static_assert(std::is_same<function_traits<decltype(&A::dummy2)>::argument<3>::type, void const*>::value, "");

    // functor
    struct B {
        bool operator()(double, int, int[], char *) const;
    };
    static_assert(function_traits<B>::argument_size == 5, "");
    static_assert(std::is_same<function_traits<B>::argument<0>::type, B&>::value, "");
    static_assert(std::is_same<function_traits<B>::argument<1>::type, double>::value, "");
    static_assert(std::is_same<function_traits<B>::argument<3>::type, int*>::value, "");
}

LSF_TEST_CASE(test_traits) {
    struct A {
        void dummy(int, int&, int*);
    };
    using F1 = int(int&);
    using F1P = int(*)(int&);
    using F2 = decltype(&A::dummy);

    static_assert(!std::is_same<typename std::add_pointer<F1>::type, int(int&)>::value, "");
    static_assert(std::is_same<typename std::add_pointer<F1>::type, F1P>::value, "");
    static_assert(std::is_same<F2, void(A::*)(int, int&, int*)>::value, "");

}

int main(int argc, char** argv) { LSF_TEST_ALL(argc, argv); }

// vim:ts=4:sw=4:et:ft=cpp:
