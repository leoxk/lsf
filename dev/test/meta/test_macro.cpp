// File:        test_macro.cpp
// Description: ---
// Notes:       ---
// Author:      leoxiang <leoxiang@tencent.com>
// Revision:    2012-11-07 by leoxiang

#include "lsf/meta/macro.hpp"
#include "lsf/basic/unit_test.hpp"

using namespace std;

LSF_TEST_CASE(test_basic_macro)
{
    // test token cat
    LSF_ASSERT(LSF_TOKEN_CAT(123, 456) == 123456);

    // test token to string
    LSF_ASSERT(LSF_TOKEN_TO_STRING(123456) == string("123456"));
    LSF_ASSERT(LSF_TOKEN_TO_STRING(LSF_TOKEN_CAT(123, 456)) == string("123456"));

    // test count inc
    LSF_ASSERT(LSF_CNT_INC(0)   == 1);
    LSF_ASSERT(LSF_CNT_INC(100) == 101);
    LSF_ASSERT(LSF_CNT_INC(511) == 512);

    // test count dec
    LSF_ASSERT(LSF_CNT_DEC(1)   == 0);
    LSF_ASSERT(LSF_CNT_DEC(100) == 99);
    LSF_ASSERT(LSF_CNT_DEC(512) == 511);
}

LSF_TEST_CASE(test_more_macro)
{
    // test repeat
    LSF_ASSERT(LSF_TOKEN_TO_STRING(LSF_REPEAT(5, m, l, p)) == 
            string("m(1,p) m(2,p) m(3,p) m(4,p) l(5,p)"));
    
    LSF_ASSERT(LSF_TOKEN_TO_STRING(LSF_REPEAT_NEST(5, m, l, p)) == 
            string("m(1,p) m(2,p) m(3,p) m(4,p) l(5,p)"));
    
    // test template params
    LSF_ASSERT(LSF_TOKEN_TO_STRING((LSF_TPARAMS(5))) == 
            string("(typename T1, typename T2, typename T3, typename T4, typename T5)"));

    LSF_ASSERT(LSF_TOKEN_TO_STRING((LSF_TPARAMS_DEF(5, def))) ==
            string("(typename T1 = def, typename T2 = def, typename T3 = def, typename T4 = def, typename T5 = def)"));

    // test args
    LSF_ASSERT(LSF_TOKEN_TO_STRING((LSF_ARGS(5, T)))   == string("(T1, T2, T3, T4, T5)"));
    LSF_ASSERT(LSF_TOKEN_TO_STRING((LSF_ARGS_S(5, T))) == string("(T2, T3, T4, T5, T6)"));

    // test typedefs
    LSF_ASSERT(LSF_TOKEN_TO_STRING((LSF_TYPEDEFS(5))) == 
            string("(typedef T1 type1; typedef T2 type2; typedef T3 type3; typedef T4 type4; typedef T5 type5;)"));

    LSF_ASSERT(LSF_TOKEN_TO_STRING((LSF_TYPEDEFS_D(5, type))) == 
            string("(typedef type type1; typedef type type2; typedef type type3; typedef type type4; typedef type type5;)"));

    // test list items
    LSF_ASSERT(LSF_TOKEN_TO_STRING((LSF_LIST_ITEMS(5, type))) == string( "(type, type, type, type, type)"));

    // test tuple params
    LSF_ASSERT(LSF_TOKEN_TO_STRING((LSF_REPEAT_NEST(5, LSF_TUPLE_DATA_CTOR, LSF_TUPLE_DATA_CTOR, arg))) == string(
            "(TupleData(typename detail::TupleParam<L,N+1>::type arg1) : head(arg1), tail() { }"
            " TupleData(typename detail::TupleParam<L,N+1>::type arg1, typename detail::TupleParam<L,N+2>::type arg2) : head(arg1), tail(arg2) { }"
            " TupleData(typename detail::TupleParam<L,N+1>::type arg1, typename detail::TupleParam<L,N+2>::type arg2, typename detail::TupleParam<L,N+3>::type arg3) : head(arg1), tail(arg2, arg3) { }"
            " TupleData(typename detail::TupleParam<L,N+1>::type arg1, typename detail::TupleParam<L,N+2>::type arg2, typename detail::TupleParam<L,N+3>::type arg3, typename detail::TupleParam<L,N+4>::type arg4) : head(arg1), tail(arg2, arg3, arg4) { }"
            " TupleData(typename detail::TupleParam<L,N+1>::type arg1, typename detail::TupleParam<L,N+2>::type arg2, typename detail::TupleParam<L,N+3>::type arg3, typename detail::TupleParam<L,N+4>::type arg4, typename detail::TupleParam<L,N+5>::type arg5) : head(arg1), tail(arg2, arg3, arg4, arg5) { })"));

    LSF_ASSERT(LSF_TOKEN_TO_STRING((LSF_REPEAT_NEST(5, LSF_TUPLE_CTOR, LSF_TUPLE_CTOR, arg))) == string(
            "(Tuple(typename detail::TupleParam<list_type,1>::type arg1) : base_type(arg1) { }"
            " Tuple(typename detail::TupleParam<list_type,1>::type arg1, typename detail::TupleParam<list_type,2>::type arg2) : base_type(arg1, arg2) { }"
            " Tuple(typename detail::TupleParam<list_type,1>::type arg1, typename detail::TupleParam<list_type,2>::type arg2, typename detail::TupleParam<list_type,3>::type arg3) : base_type(arg1, arg2, arg3) { }"
            " Tuple(typename detail::TupleParam<list_type,1>::type arg1, typename detail::TupleParam<list_type,2>::type arg2, typename detail::TupleParam<list_type,3>::type arg3, typename detail::TupleParam<list_type,4>::type arg4) : base_type(arg1, arg2, arg3, arg4) { }"
            " Tuple(typename detail::TupleParam<list_type,1>::type arg1, typename detail::TupleParam<list_type,2>::type arg2, typename detail::TupleParam<list_type,3>::type arg3, typename detail::TupleParam<list_type,4>::type arg4, typename detail::TupleParam<list_type,5>::type arg5) : base_type(arg1, arg2, arg3, arg4, arg5) { })"));

    //cout << LSF_TOKEN_TO_STRING((LSF_REPEAT_NEST(5, LSF_TUPLE_CTOR, LSF_TUPLE_CTOR, arg))) << endl;

    // test func params
    //cout << LSF_TOKEN_TO_STRING((LSF_FUNC_PARAMS(5, test))) << endl;
}

int main(int argc, char **argv)
{
	LSF_TEST_ALL();
}

// vim:ts=4:sw=4:et:ft=cpp:
