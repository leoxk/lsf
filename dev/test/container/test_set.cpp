// File:        test_set.cpp
// Description: ---
// Notes:       ---
// Author:      leoxiang <leoxiang@tencent.com>
// Revision:    2012-08-01 by leoxiang

#include "lsf/basic/unit_test.hpp"
#include "lsf/util/random.hpp"
#include "lsf/container/set.hpp"
#include "lsf/util/shared_mem.hpp"
#include "node.hpp"
#include <set>

using namespace std;
using namespace lsf::container;
using namespace lsf::util;

#define SHM_KEY  0x082157ff
#define QUEUE_SIZE 1024

#define PRINT_ALL \
    do { \
        cout << *sets.FindRoot() << endl; \
        cout << sets.ToString(0) << endl; \
        cout << sets.ToString(1) << endl; \
        cout << sets.ToString(2) << endl; \
        cout << sets.ToString(3) << endl; \
        cout << sets.ToString(4) << endl; \
        cout << sets.ToString(5) << endl; \
        cout << sets.ToString(6) << endl; \
        cout << sets.ToString(7) << endl; \
        cout << sets.ToString(8) << endl; \
        cout << sets.ToString(9) << endl; \
        cout << sets.ToString(10) << endl; \
    } while (0)


LSF_TEST_CASE(easy_test)
{
    // test bind storage and initiate
    Set<TestNode, SharedMem> sets;
    if (SharedMem::IsShmExist(SHM_KEY)) LSF_ASSERT(SharedMem::Delete(SHM_KEY));
    LSF_ASSERT(SharedMem::Create(SHM_KEY, Set<TestNode, SharedMem>::CalcByteSize(10)));
    LSF_ASSERT(sets.BindStorage(SharedMem(SHM_KEY)));
    LSF_ASSERT(sets.InitStorage());
    LSF_ASSERT(sets.MaxSize() == 10);

    // test insert
    LSF_ASSERT(sets.IsEmpty());
    LSF_ASSERT(sets.Insert(TestNode(1, 1)));
    LSF_ASSERT(sets.Insert(TestNode(2, 2)));
    LSF_ASSERT(sets.Insert(TestNode(3, 3)));
    LSF_ASSERT(sets.Insert(TestNode(4, 4)));
    LSF_ASSERT(sets.Insert(TestNode(5, 5)));
    LSF_ASSERT(5 == sets.Size());

    LSF_ASSERT(!sets.Insert(TestNode(5, 5)));
    LSF_ASSERT(!sets.Insert(TestNode(2, 2)));
    LSF_ASSERT(!sets.Insert(TestNode(1, 1)));

    LSF_ASSERT(sets.Insert(TestNode(6, 6)));
    LSF_ASSERT(sets.Insert(TestNode(7, 7)));
    LSF_ASSERT(sets.Insert(TestNode(8, 8)));
    LSF_ASSERT(sets.Insert(TestNode(9, 9)));
    LSF_ASSERT(sets.Insert(TestNode(10, 10)));
    LSF_ASSERT(sets.IsFull());
    LSF_ASSERT(10 == sets.Size());

    LSF_ASSERT(!sets.Insert(TestNode(11, 11)));
    LSF_ASSERT(!sets.Insert(TestNode(12, 12)));
    LSF_ASSERT(!sets.Insert(TestNode(13, 13)));

    // test find
    LSF_ASSERT(*(sets.Find(TestNode(1, 1))) == TestNode(1, 1));
    LSF_ASSERT(*(sets.Find(TestNode(2, 2))) == TestNode(2, 2));
    LSF_ASSERT(*(sets.Find(TestNode(3, 3))) == TestNode(3, 3));
    LSF_ASSERT(*(sets.Find(TestNode(4, 4))) == TestNode(4, 4));
    LSF_ASSERT(*(sets.Find(TestNode(5, 5))) == TestNode(5, 5));
    LSF_ASSERT(*(sets.Find(TestNode(6, 6))) == TestNode(6, 6));
    LSF_ASSERT(*(sets.Find(TestNode(7, 7))) == TestNode(7, 7));
    LSF_ASSERT(*(sets.Find(TestNode(8, 8))) == TestNode(8, 8));
    LSF_ASSERT(*(sets.Find(TestNode(9, 9))) == TestNode(9, 9));
    LSF_ASSERT(*(sets.Find(TestNode(10, 10))) == TestNode(10, 10));
 
    // test iterator
    LSF_ASSERT(*(sets.Begin() + 0) == TestNode(1, 1));
    LSF_ASSERT(*(sets.Begin() + 1) == TestNode(2, 2));
    LSF_ASSERT(*(sets.Begin() + 2) == TestNode(3, 3));
    LSF_ASSERT(*(sets.Begin() + 3) == TestNode(4, 4));
    LSF_ASSERT(*(sets.Begin() + 4) == TestNode(5, 5));
    LSF_ASSERT(*(sets.Begin() + 5) == TestNode(6, 6));
    LSF_ASSERT(*(sets.Begin() + 6) == TestNode(7, 7));
    LSF_ASSERT(*(sets.Begin() + 7) == TestNode(8, 8));
    LSF_ASSERT(*(sets.Begin() + 8) == TestNode(9, 9));
    LSF_ASSERT(*(sets.Begin() + 9) == TestNode(10, 10));
    LSF_ASSERT(sets.Begin() + 10 == sets.End());

    LSF_ASSERT(*(sets.End() - 1)  == TestNode(10,10));
    LSF_ASSERT(*(sets.End() - 2)  == TestNode(9, 9));
    LSF_ASSERT(*(sets.End() - 3)  == TestNode(8, 8));
    LSF_ASSERT(*(sets.End() - 4)  == TestNode(7, 7));
    LSF_ASSERT(*(sets.End() - 5)  == TestNode(6, 6));
    LSF_ASSERT(*(sets.End() - 6)  == TestNode(5, 5));
    LSF_ASSERT(*(sets.End() - 7)  == TestNode(4, 4));
    LSF_ASSERT(*(sets.End() - 8)  == TestNode(3, 3));
    LSF_ASSERT(*(sets.End() - 9)  == TestNode(2, 2));
    LSF_ASSERT(*(sets.End() - 10) == TestNode(1, 1));
    LSF_ASSERT(sets.End() - 11 == sets.End());

    LSF_ASSERT(*(sets.RBegin() + 0) == TestNode(10,10));
    LSF_ASSERT(*(sets.RBegin() + 1) == TestNode(9, 9));
    LSF_ASSERT(*(sets.RBegin() + 2) == TestNode(8, 8));
    LSF_ASSERT(*(sets.RBegin() + 3) == TestNode(7, 7));
    LSF_ASSERT(*(sets.RBegin() + 4) == TestNode(6, 6));
    LSF_ASSERT(*(sets.RBegin() + 5) == TestNode(5, 5));
    LSF_ASSERT(*(sets.RBegin() + 6) == TestNode(4, 4));
    LSF_ASSERT(*(sets.RBegin() + 7) == TestNode(3, 3));
    LSF_ASSERT(*(sets.RBegin() + 8) == TestNode(2, 2));
    LSF_ASSERT(*(sets.RBegin() + 9) == TestNode(1, 1));
    LSF_ASSERT(sets.Begin() + 10 == sets.End());

    // test erase
    LSF_ASSERT(sets.Erase(TestNode(1, 1)));
    LSF_ASSERT(sets.Erase(TestNode(3, 3)));
    LSF_ASSERT(sets.Erase(TestNode(5, 5)));
    LSF_ASSERT(sets.Erase(TestNode(7, 7)));
    LSF_ASSERT(sets.Erase(TestNode(9, 9)));

    LSF_ASSERT(sets.Find(TestNode(1, 1)) == sets.End());
    LSF_ASSERT(sets.Find(TestNode(3, 3)) == sets.End());
    LSF_ASSERT(sets.Find(TestNode(5, 5)) == sets.End());
    LSF_ASSERT(sets.Find(TestNode(7, 7)) == sets.End());
    LSF_ASSERT(sets.Find(TestNode(9, 9)) == sets.End());
    LSF_ASSERT(*(sets.Find(TestNode(2, 2))) == TestNode(2, 2));
    LSF_ASSERT(*(sets.Find(TestNode(4, 4))) == TestNode(4, 4));
    LSF_ASSERT(*(sets.Find(TestNode(6, 6))) == TestNode(6, 6));
    LSF_ASSERT(*(sets.Find(TestNode(8, 8))) == TestNode(8, 8));
    LSF_ASSERT(*(sets.Find(TestNode(10, 10))) == TestNode(10, 10));

    LSF_ASSERT(sets.Erase(TestNode(2, 2)));
    LSF_ASSERT(sets.Erase(TestNode(4, 4)));
    LSF_ASSERT(sets.Erase(TestNode(6, 6)));
    LSF_ASSERT(sets.Erase(TestNode(8, 8)));
    LSF_ASSERT(sets.Erase(TestNode(10, 10)));

    LSF_ASSERT(sets.Find(TestNode(2, 2))   == sets.End());
    LSF_ASSERT(sets.Find(TestNode(4, 4))   == sets.End());
    LSF_ASSERT(sets.Find(TestNode(6, 6))   == sets.End());
    LSF_ASSERT(sets.Find(TestNode(8, 8))   == sets.End());
    LSF_ASSERT(sets.Find(TestNode(10, 10)) == sets.End());
    LSF_ASSERT(sets.IsEmpty());
    LSF_ASSERT(0 == sets.Size());

    LSF_ASSERT(SharedMem::Delete(SHM_KEY));
}

LSF_TEST_CASE(random_test)
{
    Set<TestNode, SharedMem> sets;

    size_t size = 1000000;

    if (SharedMem::IsShmExist(SHM_KEY)) LSF_ASSERT(SharedMem::Delete(SHM_KEY));
    LSF_ASSERT(SharedMem::Create(SHM_KEY, Set<TestNode, SharedMem>::CalcByteSize(size)));
    LSF_ASSERT(sets.BindStorage(SharedMem(SHM_KEY)));
    LSF_ASSERT(sets.InitStorage());

    // init a random uint group
    set<uint32_t> uint_group;

    while (uint_group.size() != size) {
        uint_group.insert(::lsf::util::SingleRandom::Reference().GetRand(0, 1000000000));
    }

    LSF_ASSERT(sets.IsEmpty());

    // test batch insert
    for (set<uint32_t>::iterator iter = uint_group.begin(); iter != uint_group.end(); iter++) {
        LSF_ASSERT_ERR_ONLY(sets.Insert(TestNode(*iter, *iter)));
    }

    LSF_ASSERT(sets.IsFull());

    // test batch find
    for (set<uint32_t>::iterator iter = uint_group.begin(); iter != uint_group.end(); iter++) {
        LSF_ASSERT_ERR_ONLY(*sets.Find(TestNode(*iter, *iter)) == TestNode(*iter, *iter));
    }

    // test batch erase
    for (set<uint32_t>::iterator iter = uint_group.begin(); iter != uint_group.end(); iter++) {
        LSF_ASSERT_ERR_ONLY(sets.Erase(TestNode(*iter, *iter)));
        LSF_ASSERT_ERR_ONLY(sets.Begin() == sets.End() + 1);
    }

    LSF_ASSERT(sets.IsEmpty());

    LSF_ASSERT(SharedMem::Delete(SHM_KEY));
}

int main(int argc, char **argv)
{
	LSF_TEST_ALL();
}

// vim:ts=4:sw=4:et:ft=cpp:
