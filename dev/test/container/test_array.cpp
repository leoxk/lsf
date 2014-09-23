// File:        test_array.cpp
// Description: ---
// Notes:       ---
// Author:      leoxiang <leoxiang@tencent.com>
// Revision:    2012-12-13 by leoxiang

#include "lsf/basic/unit_test.hpp"
#include "lsf/container/array.hpp"
#include "lsf/util/shared_mem.hpp"
#include "node.hpp"

using namespace std;
using namespace lsf::container;
using namespace lsf::util;

#define SHM_KEY  0x082157ff
#define ARRAY_SIZE 10

LSF_TEST_CASE(test_common)
{
    Array<TestNode, SharedMem> array;

    if (SharedMem::IsShmExist(SHM_KEY)) LSF_ASSERT(SharedMem::Delete(SHM_KEY));
    LSF_ASSERT(SharedMem::Create(SHM_KEY, array.CalcByteSize(ARRAY_SIZE)));

    LSF_ASSERT(array.BindStorage(SharedMem(SHM_KEY)));
    LSF_ASSERT(array.InitStorage());
    LSF_ASSERT(array.MaxSize() == ARRAY_SIZE);
    LSF_ASSERT(array.IsBindStorage());

    LSF_ASSERT(array.Get(0) == TestNode());
    LSF_ASSERT(array.Get(1) == TestNode());
    LSF_ASSERT(array.Get(2) == TestNode());

    LSF_ASSERT(*array.GetPtr(3) == TestNode());
    LSF_ASSERT(*array.GetPtr(4) == TestNode());
    LSF_ASSERT(*array.GetPtr(5) == TestNode());

    LSF_ASSERT(array.operator[](6) == TestNode());
    LSF_ASSERT(array.operator[](7) == TestNode());
    LSF_ASSERT(array.operator[](8) == TestNode());
    LSF_ASSERT(array.operator[](9) == TestNode());

    LSF_ASSERT(array.GetPtr(10) == NULL);
    LSF_ASSERT(array.GetPtr(11) == NULL);
}

LSF_TEST_CASE(test_iterator)
{
    typedef Array<TestNode, SharedMem> array_type;
    array_type array;

    if (SharedMem::IsShmExist(SHM_KEY)) LSF_ASSERT(SharedMem::Delete(SHM_KEY));
    LSF_ASSERT(SharedMem::Create(SHM_KEY, array.CalcByteSize(ARRAY_SIZE)));
    LSF_ASSERT(array.BindStorage(SharedMem(SHM_KEY)));
    LSF_ASSERT(array.InitStorage());

    for (array_type::iterator iter = array.Begin(); iter != array.End(); iter++) {
        iter->key = 8;
        iter->data = 88;
    }

    for (array_type::reverse_iterator riter = array.RBegin(); riter != array.REnd(); riter++) {
        LSF_ASSERT(*riter == TestNode(8, 88));
    }
}

int main(int argc, char **argv)
{
	LSF_TEST_ALL();
}

// vim:ts=4:sw=4:et:ft=cpp:
