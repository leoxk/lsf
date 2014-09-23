// File:        test_type_traits.cpp
// Description: ---
// Notes:       ---
// Author:      leoxiang <leoxiang@tencent.com>
// Revision:    2012-11-08 by leoxiang

#include "lsf/meta/type_traits.hpp"
#include "lsf/basic/unit_test.hpp"

using namespace std;
using namespace lsf::meta;

LSF_TEST_CASE(test)
{
    // test is_same
    LSF_ASSERT((IsSame<int, int>::value));
    LSF_ASSERT((!IsSame<int, short>::value));

    typedef int my_type;
    LSF_ASSERT((IsSame<int, my_type>::value));

    // test remove cv
    LSF_ASSERT((IsSame<int, RemoveCV<const int>::type>::value));
    LSF_ASSERT((IsSame<int, RemoveCV<const volatile int>::type>::value));
    LSF_ASSERT((IsSame<int &, RemoveCV<const int &>::type>::value));
    LSF_ASSERT((IsSame<int &, RemoveCV<const volatile int &>::type>::value));
    LSF_ASSERT((IsSame<int *, RemoveCV<const int *>::type>::value));
    LSF_ASSERT((IsSame<int *, RemoveCV<const volatile int *>::type>::value));

    // test is const
    LSF_ASSERT((!IsConst<int>::value));
    LSF_ASSERT((IsConst<const int>::value));
    LSF_ASSERT((IsConst<int const &>::value));
    LSF_ASSERT((IsConst<int const *>::value));

    // test is_reference
    LSF_ASSERT((!IsReference<int>::value));
    LSF_ASSERT((IsReference<int &>::value));
    LSF_ASSERT((IsReference<int const &>::value));

    LSF_ASSERT((!IsConstReference<int &>::value));
    LSF_ASSERT((IsConstReference<int const &>::value));

    // test is_pointer
    LSF_ASSERT((!IsPointer<int>::value));
    LSF_ASSERT((IsPointer<int *>::value));
    LSF_ASSERT((IsPointer<int const *>::value));

    LSF_ASSERT((!IsConstPointer<int *>::value));
    LSF_ASSERT((IsConstPointer<int const *>::value));

    // test is_assignable
    LSF_ASSERT((IsAssignable<int, int        >::value));
    LSF_ASSERT((IsAssignable<int, const int  >::value));
    LSF_ASSERT((IsAssignable<int, int &      >::value));
    LSF_ASSERT((IsAssignable<int, int const &>::value));
    LSF_ASSERT((IsAssignable<int &, int  >::value));
    LSF_ASSERT((IsAssignable<int &, int &>::value));
    LSF_ASSERT((!IsAssignable<int const &, int        >::value));
    LSF_ASSERT((!IsAssignable<int const &, int const  >::value));
    LSF_ASSERT((!IsAssignable<int const &, int &      >::value));
    LSF_ASSERT((!IsAssignable<int const &, int const &>::value));
}

int main(int argc, char **argv)
{
	LSF_TEST_ALL();
}

// vim:ts=4:sw=4:et:ft=cpp:
