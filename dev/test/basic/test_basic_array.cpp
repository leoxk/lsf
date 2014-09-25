// File:        test_basic_array.cpp
// Description: ---
// Notes:       ---
// Author:      leoxiang <leoxiang@tencent.com>
// Revision:    2012-06-01 by leoxiang

#include "lsf/basic/unit_test.hpp"
#include "lsf/basic/array.hpp"

using namespace std;
using namespace lsf::basic;

struct TestType
{
    TestType() { }
    TestType(int _key, int _data) : key(_key), data(_data) { }
    bool operator==(TestType const & rhs) { return key == rhs.key && data == rhs.data; }

    int  key;
    int  data;
};

LSF_TEST_CASE(test_array_with_pod_type)
{
    Array<int>     array(10);
    Array<int>     array_copy(10);

    LSF_ASSERT(array.Size() == 10);
    LSF_ASSERT(array.ByteSize() == sizeof(int) * 10);

    // test fill
    array.Fill(88);
    for (Array<int>::iterator iter = array.Begin(); iter != array.End(); iter++) {
        LSF_ASSERT(*iter == 88);
    }

    // test copy assignment
    array_copy = array;
    LSF_ASSERT(array_copy == array);
    LSF_ASSERT(array.Size() == array_copy.Size());
    LSF_ASSERT(array.Data() != array_copy.Data());

    // test copy raw data
    memcpy(array_copy.Data(), array.Data(), array.ByteSize());
    LSF_ASSERT(array_copy == array);
}

LSF_TEST_CASE(test_array_with_class_type)
{
    Array<TestType>      array;
    Array<TestType>      array_copy;

    array.Init(10);
    LSF_ASSERT(array.Size() == 10);
    LSF_ASSERT(array.ByteSize() == sizeof(TestType) * 10);

    array.Fill(TestType(1,1));
    for (StaticArray<TestType,10>::iterator iter = array.Begin(); iter != array.End(); iter++) {
        LSF_ASSERT(*iter == TestType(1,1));
    }

    array_copy = array;
    LSF_ASSERT(array_copy == array);

    memcpy(array_copy.Data(), array.Data(), array.ByteSize());
    LSF_ASSERT(array_copy == array);
}

LSF_TEST_CASE(test_static_array_with_pod_type)
{
    StaticArray<int,10>     array;
    StaticArray<int,10>     array_copy;

    LSF_ASSERT(array.Size() == 0);

    array.Fill(88);
    for (StaticArray<int,10>::iterator iter = array.Begin(); iter != array.End(); iter++) {
        LSF_ASSERT(*iter == 88);
    }

    array_copy = array;
    LSF_ASSERT(array_copy == array);

    array_copy.Copy(array.Begin(), array.End());
    LSF_ASSERT(array_copy == array);
}

LSF_TEST_CASE(test_static_array_with_class_type)
{
    StaticArray<TestType,10>      array;
    StaticArray<TestType,10>      array_copy;

    LSF_ASSERT(array.Size() == 0);

    array.Fill(TestType(1,1));
    for (StaticArray<TestType,10>::iterator iter = array.Begin(); iter != array.End(); iter++) {
        LSF_ASSERT(*iter == TestType(1,1));
    }

    array_copy = array;
    LSF_ASSERT(array_copy == array);

    array_copy.Copy(array.Begin(), array.End());
    LSF_ASSERT(array_copy == array);
}

int main(int argc, char **argv)
{
	LSF_TEST_ALL();
}

// vim:ts=4:sw=4:et:ft=cpp:
