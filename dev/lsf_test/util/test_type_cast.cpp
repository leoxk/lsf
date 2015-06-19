// File:        test_type_cast.cpp
// Description: ---
// Notes:       ---
// Author:      leoxiang <leoxiang727@qq.com>
// Revision:    2012-01-31 by leoxiang

#include "lsf/basic/unit_test.hpp"
#include "lsf/util/type_cast.hpp"

using namespace std;
using namespace lsf::util;

LSF_TEST_CASE(cast_string_to_number)
{
    // string to int
    LSF_ASSERT(TypeCast<int>(string("123"))   == (int)123);
    LSF_ASSERT(TypeCast<int>(string("-123"))  == (int)-123);
    LSF_ASSERT(TypeCast<int>(string("0xff"))  == (int)255);
    LSF_ASSERT(TypeCast<int>(string("-0xff")) == (int)-255);
    LSF_ASSERT(TypeCast<int>(string("077"))   == (int)63);
    LSF_ASSERT(TypeCast<int>(string("-077"))  == (int)-63);

    // char * to int
    LSF_ASSERT(TypeCast<int>("123")   == (int)123);
    LSF_ASSERT(TypeCast<int>("-123")  == (int)-123);
    LSF_ASSERT(TypeCast<int>("0xff")  == (int)255);
    LSF_ASSERT(TypeCast<int>("-0xff") == (int)-255);
    LSF_ASSERT(TypeCast<int>("077")   == (int)63);
    LSF_ASSERT(TypeCast<int>("-077")  == (int)-63);

    // string to uint
    LSF_ASSERT(TypeCast<unsigned int>(string("123"))  == (unsigned int)123);
    LSF_ASSERT(TypeCast<unsigned int>(string("0xff")) == (unsigned int)255);
    LSF_ASSERT(TypeCast<unsigned int>(string("077"))  == (unsigned int)63);

    // string to short
    LSF_ASSERT(TypeCast<short>(string("123"))  == (short)123);
    LSF_ASSERT(TypeCast<short>(string("0xff")) == (short)255);
    LSF_ASSERT(TypeCast<short>(string("077"))  == (short)63);

    // string to unsigned short 
    LSF_ASSERT(TypeCast<unsigned short>(string("123"))  == (unsigned short)123);
    LSF_ASSERT(TypeCast<unsigned short>(string("0xff")) == (unsigned short)255);
    LSF_ASSERT(TypeCast<unsigned short>(string("077"))  == (unsigned short)63);

    // string to long long
    //LSF_ASSERT(TypeCast<long long>(string("0x12300000000"))  == ((long long)0x123 << 32));
    //LSF_ASSERT(TypeCast<long long>(string("1249835483136"))  == ((long long)0x123 << 32));
    
    // string to unsigned long long
    //LSF_ASSERT(TypeCast<unsigned long long>(string("0x12300000000"))  == ((long long)0x123 << 32));
    //LSF_ASSERT(TypeCast<unsigned long long>(string("1249835483136"))  == ((long long)0x123 << 32));
    //LSF_ASSERT(TypeCast<unsigned long long>(string("18620300027"))  == ((long long)18620300027LL));

    // char* to unsigned long long
    //uint64_t uint = TypeCast<uint64_t>("18620300027");
    //LSF_ASSERT(uint == ((long long)18620300027LL));
    //LSF_ASSERT(TypeCast<uint64_t>("18620300027")  == ((long long)18620300027LL));
}

LSF_TEST_CASE(cast_between_number)
{
    // int to uint
    LSF_ASSERT(TypeCast<unsigned int>((int)0x7fffffff) == (unsigned int)0x7fffffff);
    LSF_ASSERT(TypeCast<unsigned int>((int)0x8fffffff) == (unsigned int)0x8fffffff);
    
    // uint to int
    LSF_ASSERT(TypeCast<int>((unsigned int)0x7fffffff) == (int)0x7fffffff);
    LSF_ASSERT(TypeCast<int>((unsigned int)0x8fffffff) == (int)0x8fffffff);
}

//LSF_TEST_CASE(cast_number_to_string)
//{
    //// int to string
    //LSF_ASSERT(TypeCast<string>      ((int)123) == string("123"));
    //LSF_ASSERT(TypeCast<char const *>((int)123) == string("123"));
    //LSF_ASSERT(TypeCast<char *>      ((int)123) == string("123"));

    //// uint to string
    //LSF_ASSERT(TypeCast<string>((unsigned int)123)  == string("123"));
    //LSF_ASSERT(TypeCast<string>((unsigned int)0xff)  == string("255"));
    //LSF_ASSERT(TypeCast<string>((unsigned int)077)   == string("63"));

    //// short to string
    //LSF_ASSERT(TypeCast<string>((short)123)   == string("123"));
    //LSF_ASSERT(TypeCast<string>((short)-123)  == string("-123"));
    //LSF_ASSERT(TypeCast<string>((short)0xff)  == string("255"));
    //LSF_ASSERT(TypeCast<string>((short)-0xff) == string("-255"));
    //LSF_ASSERT(TypeCast<string>((short)077)   == string("63"));
    //LSF_ASSERT(TypeCast<string>((short)-077)  == string("-63"));

    //// unsigned short to string
    //LSF_ASSERT(TypeCast<string>((unsigned short)123)   == string("123"));
    //LSF_ASSERT(TypeCast<string>((unsigned short)0xff)  == string("255"));
    //LSF_ASSERT(TypeCast<string>((unsigned short)077)   == string("63"));

    //// long long to string
    //LSF_ASSERT(TypeCast<string>((long long)0x123 << 32) == string("1249835483136"));

    //// unsigned long long to string
    //LSF_ASSERT(TypeCast<string>((unsigned long long)0x123 << 32) == string("1249835483136"));

    //// char to string
    //LSF_ASSERT(TypeCast<string>('c')  == string("c"));
    //LSF_ASSERT(TypeCast<string>('\n') == string("\n"));
//}

LSF_TEST_CASE(cast_between_string)
{

}

int main(int argc, char** argv)
{
    LSF_TEST_ALL(argc, argv);
}


// vim:ts=4:sw=4:et:ft=cpp:
