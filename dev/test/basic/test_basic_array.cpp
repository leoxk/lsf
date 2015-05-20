// File:        test_basic_array.cpp
// Description: ---
// Notes:       ---
// Author:      leoxiang <leoxiang727@qq.com>
// Revision:    2012-06-01 by leoxiang

#include "lsf/basic/unit_test.hpp"
#include "lsf/basic/array.hpp"
#include "lsf/basic/type_cast.hpp"

using namespace std;
using namespace lsf::basic;

struct TestType
{
    TestType() { }
    TestType(int _key, int _data) : key(_key), data(_data) { }
    bool operator==(TestType const & rhs) { return key == rhs.key && data == rhs.data; }

    string ToString() const { return TypeCast<string>(key); }

    int  key;
    int  data;
};

LSF_TEST_CASE(test_array_with_pod_type)
{
    Array<int,10>     array;
    Array<int,10>     array_copy;

    LSF_ASSERT(array.Size() == 0);

    array.Fill(88);
    for (Array<int,10>::iterator iter = array.Begin(); iter != array.End(); iter++) {
        LSF_ASSERT(*iter == 88);
    }

    array_copy = array;
    LSF_ASSERT(array_copy == array);

    // test copy raw data
    array_copy.Copy(array.Begin(), array.End());
    LSF_ASSERT(array_copy == array);
}

LSF_TEST_CASE(test_array_with_class_type)
{
    Array<TestType,10>      array;
    Array<TestType,10>      array_copy;

    LSF_ASSERT(array.Size() == 0);

    array.Fill(TestType(1,1));
    for (Array<TestType,10>::iterator iter = array.Begin(); iter != array.End(); iter++) {
        LSF_ASSERT(*iter == TestType(1,1));
    }

    array_copy = array;
    LSF_ASSERT(array_copy == array);

    array_copy.Copy(array.Begin(), array.End());
    LSF_ASSERT(array_copy == array);
}

LSF_TEST_CASE(test_erase)
{
    Array<TestType,10>      array;
    array.PushBack(TestType(0,0));
    array.PushBack(TestType(1,1));
    array.PushBack(TestType(2,2));
    array.PushBack(TestType(3,3));
    array.PushBack(TestType(4,4));

    LSF_ASSERT(array.ToString() == "0,1,2,3,4");
    LSF_ASSERT(array[0] == TestType(0,0));
    LSF_ASSERT(array[1] == TestType(1,1));
    LSF_ASSERT(array[2] == TestType(2,2));
    LSF_ASSERT(array[3] == TestType(3,3));
    LSF_ASSERT(array[4] == TestType(4,4));
    LSF_ASSERT(array.Size() == 5);

    LSF_ASSERT(array.Erase(array.Begin()+4) == array.End());
    LSF_ASSERT(array.Erase(array.End()-1) == array.End());

    LSF_ASSERT(array.Erase(array.Begin(), array.End()) == array.End());
    LSF_ASSERT(array.IsEmpty());
}

int main(int argc, char **argv)
{
	LSF_TEST_ALL();
}

// vim:ts=4:sw=4:et:ft=cpp:
