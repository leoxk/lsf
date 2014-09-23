// File:        test_type_list.cpp
// Description: ---
// Notes:       ---
// Author:      leoxiang <leoxiang@tencent.com>
// Revision:    2012-11-09 by leoxiang

#include "lsf/meta/type_list.hpp"
#include "lsf/meta/type_traits.hpp"
#include "lsf/basic/unit_test.hpp"

using namespace std;
using namespace lsf::meta;

LSF_TEST_CASE(test)
{
    typedef TypeList <
        int, 
        const int, 
        int &, 
        int const &, 
        int *, 
        int const *, 
        void *, 
        void const *
            >           list_type;
    
    // test GetType
    LSF_ASSERT(list_type::size == 8);
    LSF_ASSERT((IsSame<int         , GetType<list_type, 0>::type>::value));
    LSF_ASSERT((IsSame<const int   , GetType<list_type, 1>::type>::value));
    LSF_ASSERT((IsSame<int &       , GetType<list_type, 2>::type>::value));
    LSF_ASSERT((IsSame<int const & , GetType<list_type, 3>::type>::value));
    LSF_ASSERT((IsSame<int *       , GetType<list_type, 4>::type>::value));
    LSF_ASSERT((IsSame<int const * , GetType<list_type, 5>::type>::value));
    LSF_ASSERT((IsSame<void *      , GetType<list_type, 6>::type>::value));
    LSF_ASSERT((IsSame<void const *, GetType<list_type, 7>::type>::value));

    // test FindType
    LSF_ASSERT((IsSame<FindType<TypeList<int>, int>::type, int>::value));
    LSF_ASSERT((IsSame<FindType<TypeList<int &>, int &>::type, int &>::value));
    LSF_ASSERT((IsSame<FindType<TypeList<int const &>, int const &>::type, int const &>::value));
}

int main(int argc, char **argv)
{
	LSF_TEST_ALL();
}

// vim:ts=4:sw=4:et:ft=cpp:
