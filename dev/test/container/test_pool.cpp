// File:        test_pool.cpp
// Description: ---
// Notes:       ---
// Author:      leoxiang <leoxiang727@qq.com>
// Revision:    2012-08-01 by leoxiang

#include "lsf/basic/unit_test.hpp"
#include "lsf/basic/type_cast.hpp"
#include "lsf/container/pool.hpp"
#include "node.hpp"

using namespace std;
using namespace lsf::basic;
using namespace lsf::container;

#define SHM_KEY  0x082157ff
#define QUEUE_SIZE 1024

LSF_TEST_CASE(test_pool)
{
    Pool<TestNode, SharedMem> pool;
    LSF_ASSERT(SharedMem::Create(SHM_KEY, Pool<TestNode, SharedMem>::CalcByteSize(10)));
    if (SharedMem::IsShmExist(SHM_KEY)) LSF_ASSERT(SharedMem::Delete(SHM_KEY));
    LSF_ASSERT(SharedMem::Create(SHM_KEY, Pool<TestNode, SharedMem>::CalcByteSize(10)));
    LSF_ASSERT(pool.BindAndInitStorage(SharedMem(SHM_KEY)));
    LSF_ASSERT(pool.MaxSize() == 10);

    // test malloc
    size_t pos_arr[10];
    pos_arr[0] = pool.Malloc(TestNode(0, 0));
    pos_arr[1] = pool.Malloc(TestNode(1, 1));
    pos_arr[2] = pool.Malloc(TestNode(2, 2));
    pos_arr[3] = pool.Malloc(TestNode(3, 3));
    pos_arr[4] = pool.Malloc(TestNode(4, 4)); // 1 2 3 4 5

    // test get
    pos_arr[5] = pool.Malloc(); 
    pos_arr[6] = pool.Malloc(); 
    pos_arr[7] = pool.Malloc(); 
    pos_arr[8] = pool.Malloc(); 
    pos_arr[9] = pool.Malloc(); 
    pool.Get(pos_arr[5]) = TestNode(5, 5);
    pool.Get(pos_arr[6]) = TestNode(6, 6);
    pool.Get(pos_arr[7]) = TestNode(7, 7);
    pool.Get(pos_arr[8]) = TestNode(8, 8);
    pool.Get(pos_arr[9]) = TestNode(9, 9); // 0 1 2 3 4 5 6 7 8 9
    LSF_ASSERT(NULL != pool.GetPtr(pool.MaxSize()));
    LSF_ASSERT(NULL == pool.GetPtr(pool.MaxSize() + 1));
    LSF_ASSERT(TestNode(9, 9) == pool.Get(pool.MaxSize()));

    // test iterator
    LSF_ASSERT(*(pool.Begin() + 0) == TestNode(0, 0));
    LSF_ASSERT(*(pool.Begin() + 1) == TestNode(1, 1));
    LSF_ASSERT(*(pool.Begin() + 2) == TestNode(2, 2));
    LSF_ASSERT(*(pool.Begin() + 3) == TestNode(3, 3));
    LSF_ASSERT(*(pool.Begin() + 4) == TestNode(4, 4));
    LSF_ASSERT(pool.Begin() + 10 == pool.End());
    LSF_ASSERT(pool.Begin() != pool.End());

    // test reverse iterator
    LSF_ASSERT(*(pool.RBegin() + 0) == TestNode(9, 9));
    LSF_ASSERT(*(pool.RBegin() + 1) == TestNode(8, 8));
    LSF_ASSERT(*(pool.RBegin() + 2) == TestNode(7, 7));
    LSF_ASSERT(*(pool.RBegin() + 3) == TestNode(6, 6));
    LSF_ASSERT(*(pool.RBegin() + 4) == TestNode(5, 5));
    LSF_ASSERT(pool.RBegin() + 10 == pool.REnd());

    // test free
    LSF_ASSERT(pool.Free(pos_arr[1]));
    LSF_ASSERT(pool.Free(pos_arr[3]));
    LSF_ASSERT(pool.Free(pos_arr[5]));
}

LSF_TEST_CASE(test_used)
{
    Pool<TestNode, SharedMem> pool;
    LSF_ASSERT(SharedMem::Create(SHM_KEY, Pool<TestNode, SharedMem>::CalcByteSize(10)));
    if (SharedMem::IsShmExist(SHM_KEY)) LSF_ASSERT(SharedMem::Delete(SHM_KEY));
    LSF_ASSERT(SharedMem::Create(SHM_KEY, Pool<TestNode, SharedMem>::CalcByteSize(10)));
    LSF_ASSERT(pool.BindAndInitStorage(SharedMem(SHM_KEY)));

    // test malloc
    size_t pos_arr[10];
    pos_arr[0] = pool.Malloc(TestNode(0, 0));
    pos_arr[1] = pool.Malloc(TestNode(1, 1));
    pos_arr[2] = pool.Malloc(TestNode(2, 2));
    pos_arr[3] = pool.Malloc(TestNode(3, 3));
    pos_arr[4] = pool.Malloc(TestNode(4, 4)); // 1 2 3 4 5

    // test used
    LSF_ASSERT(pool.GetPtr(pos_arr[0]) != NULL);
    LSF_ASSERT(pool.GetPtr(pos_arr[1]) != NULL);
    LSF_ASSERT(pool.GetPtr(pos_arr[2]) != NULL);
    LSF_ASSERT(pool.GetPtr(pos_arr[3]) != NULL);
    LSF_ASSERT(pool.GetPtr(pos_arr[4]) != NULL);

    // iterator
    LSF_ASSERT(*(pool.Begin() + 0) == TestNode(0, 0));
    LSF_ASSERT(*(pool.Begin() + 1) == TestNode(1, 1));
    LSF_ASSERT(*(pool.Begin() + 2) == TestNode(2, 2));
    LSF_ASSERT(*(pool.Begin() + 3) == TestNode(3, 3));
    LSF_ASSERT(*(pool.Begin() + 4) == TestNode(4, 4));
    LSF_ASSERT(pool.Begin() + pool.Size() == pool.End());

    // test free
    LSF_ASSERT(pool.GetPtr(pos_arr[0]) != NULL);
    LSF_ASSERT(pool.GetPtr(pos_arr[2]) != NULL);
    LSF_ASSERT(pool.GetPtr(pos_arr[4]) != NULL);
    LSF_ASSERT(pool.Free(pos_arr[0]));
    LSF_ASSERT(pool.Free(pos_arr[2]));
    LSF_ASSERT(pool.Free(pos_arr[4]));
    LSF_ASSERT(pool.GetPtr(pos_arr[0]) == NULL);
    LSF_ASSERT(pool.GetPtr(pos_arr[2]) == NULL);
    LSF_ASSERT(pool.GetPtr(pos_arr[4]) == NULL);
}

int main(int argc, char **argv)
{
	LSF_TEST_ALL();
}


// vim:ts=4:sw=4:et:ft=cpp:
