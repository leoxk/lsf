// File:        test_set.cpp
// Description: ---
// Notes:       ---
// Author:      leoxiang <leoxiang727@qq.com>
// Revision:    2012-08-01 by leoxiang

#include "lsf/basic/unit_test.hpp"
#include "lsf/util/random.hpp"
#include "lsf/container/set.hpp"
#include "node.hpp"
#include <set>

using namespace std;
using namespace lsf::container;
using namespace lsf::util;

#define SHM_KEY 0x082157ff
#define QUEUE_SIZE 1024

#define PRINT_ALL                          \
    do {                                   \
        cout << *sets.FindRoot() << endl;  \
        cout << sets.ToString(0) << endl;  \
        cout << sets.ToString(1) << endl;  \
        cout << sets.ToString(2) << endl;  \
        cout << sets.ToString(3) << endl;  \
        cout << sets.ToString(4) << endl;  \
        cout << sets.ToString(5) << endl;  \
        cout << sets.ToString(6) << endl;  \
        cout << sets.ToString(7) << endl;  \
        cout << sets.ToString(8) << endl;  \
        cout << sets.ToString(9) << endl;  \
        cout << sets.ToString(10) << endl; \
    } while (0)

LSF_TEST_CASE(easy_test) {
    // test bind storage and initiate
    Set<TestNode, SharedMem> sets;
    if (SharedMem::IsShmExist(SHM_KEY)) LSF_ASSERT(SharedMem::Delete(SHM_KEY));
    LSF_ASSERT(SharedMem::Create(SHM_KEY, Set<TestNode, SharedMem>::CalcByteSize(10)));
    LSF_ASSERT(sets.BindAndInitStorage(SharedMem(SHM_KEY)));
    LSF_ASSERT(sets.max_size() == 10);

    // test insert
    LSF_ASSERT(sets.empty());
    LSF_ASSERT(sets.Insert(TestNode(1, 1)));
    LSF_ASSERT(sets.Insert(TestNode(2, 2)));
    LSF_ASSERT(sets.Insert(TestNode(3, 3)));
    LSF_ASSERT(sets.Insert(TestNode(4, 4)));
    LSF_ASSERT(sets.Insert(TestNode(5, 5)));
    LSF_ASSERT(5 == sets.size());

    LSF_ASSERT(!sets.Insert(TestNode(5, 5)));
    LSF_ASSERT(!sets.Insert(TestNode(2, 2)));
    LSF_ASSERT(!sets.Insert(TestNode(1, 1)));

    LSF_ASSERT(sets.Insert(TestNode(6, 6)));
    LSF_ASSERT(sets.Insert(TestNode(7, 7)));
    LSF_ASSERT(sets.Insert(TestNode(8, 8)));
    LSF_ASSERT(sets.Insert(TestNode(9, 9)));
    LSF_ASSERT(sets.Insert(TestNode(10, 10)));
    LSF_ASSERT(sets.full());
    LSF_ASSERT(10 == sets.size());

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
    LSF_ASSERT(*(sets.begin() + 0) == TestNode(1, 1));
    LSF_ASSERT(*(sets.begin() + 1) == TestNode(2, 2));
    LSF_ASSERT(*(sets.begin() + 2) == TestNode(3, 3));
    LSF_ASSERT(*(sets.begin() + 3) == TestNode(4, 4));
    LSF_ASSERT(*(sets.begin() + 4) == TestNode(5, 5));
    LSF_ASSERT(*(sets.begin() + 5) == TestNode(6, 6));
    LSF_ASSERT(*(sets.begin() + 6) == TestNode(7, 7));
    LSF_ASSERT(*(sets.begin() + 7) == TestNode(8, 8));
    LSF_ASSERT(*(sets.begin() + 8) == TestNode(9, 9));
    LSF_ASSERT(*(sets.begin() + 9) == TestNode(10, 10));
    LSF_ASSERT(sets.begin() + 10 == sets.end());

    LSF_ASSERT(*(sets.end() - 1) == TestNode(10, 10));
    LSF_ASSERT(*(sets.end() - 2) == TestNode(9, 9));
    LSF_ASSERT(*(sets.end() - 3) == TestNode(8, 8));
    LSF_ASSERT(*(sets.end() - 4) == TestNode(7, 7));
    LSF_ASSERT(*(sets.end() - 5) == TestNode(6, 6));
    LSF_ASSERT(*(sets.end() - 6) == TestNode(5, 5));
    LSF_ASSERT(*(sets.end() - 7) == TestNode(4, 4));
    LSF_ASSERT(*(sets.end() - 8) == TestNode(3, 3));
    LSF_ASSERT(*(sets.end() - 9) == TestNode(2, 2));
    LSF_ASSERT(*(sets.end() - 10) == TestNode(1, 1));
    LSF_ASSERT(sets.end() - 11 == sets.end());

    LSF_ASSERT(*(sets.rbegin() + 0) == TestNode(10, 10));
    LSF_ASSERT(*(sets.rbegin() + 1) == TestNode(9, 9));
    LSF_ASSERT(*(sets.rbegin() + 2) == TestNode(8, 8));
    LSF_ASSERT(*(sets.rbegin() + 3) == TestNode(7, 7));
    LSF_ASSERT(*(sets.rbegin() + 4) == TestNode(6, 6));
    LSF_ASSERT(*(sets.rbegin() + 5) == TestNode(5, 5));
    LSF_ASSERT(*(sets.rbegin() + 6) == TestNode(4, 4));
    LSF_ASSERT(*(sets.rbegin() + 7) == TestNode(3, 3));
    LSF_ASSERT(*(sets.rbegin() + 8) == TestNode(2, 2));
    LSF_ASSERT(*(sets.rbegin() + 9) == TestNode(1, 1));
    LSF_ASSERT(sets.begin() + 10 == sets.end());

    // test erase
    LSF_ASSERT(sets.Erase(TestNode(1, 1)));
    LSF_ASSERT(sets.Erase(TestNode(3, 3)));
    LSF_ASSERT(sets.Erase(TestNode(5, 5)));
    LSF_ASSERT(sets.Erase(TestNode(7, 7)));
    LSF_ASSERT(sets.Erase(TestNode(9, 9)));

    LSF_ASSERT(sets.Find(TestNode(1, 1)) == sets.end());
    LSF_ASSERT(sets.Find(TestNode(3, 3)) == sets.end());
    LSF_ASSERT(sets.Find(TestNode(5, 5)) == sets.end());
    LSF_ASSERT(sets.Find(TestNode(7, 7)) == sets.end());
    LSF_ASSERT(sets.Find(TestNode(9, 9)) == sets.end());
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

    LSF_ASSERT(sets.Find(TestNode(2, 2)) == sets.end());
    LSF_ASSERT(sets.Find(TestNode(4, 4)) == sets.end());
    LSF_ASSERT(sets.Find(TestNode(6, 6)) == sets.end());
    LSF_ASSERT(sets.Find(TestNode(8, 8)) == sets.end());
    LSF_ASSERT(sets.Find(TestNode(10, 10)) == sets.end());
    LSF_ASSERT(sets.empty());
    LSF_ASSERT(0 == sets.size());

    LSF_ASSERT(SharedMem::Delete(SHM_KEY));
}

LSF_TEST_CASE(random_test) {
    Set<TestNode, SharedMem> sets;

    size_t size = 10000;

    if (SharedMem::IsShmExist(SHM_KEY)) LSF_ASSERT(SharedMem::Delete(SHM_KEY));
    LSF_ASSERT(SharedMem::Create(SHM_KEY, Set<TestNode, SharedMem>::CalcByteSize(size)));
    LSF_ASSERT(sets.BindAndInitStorage(SharedMem(SHM_KEY)));

    // init a random uint group
    set<uint32_t> uint_group;

    while (uint_group.size() != size) {
        uint_group.insert(::lsf::util::SingleRandom::Reference().GetRand(0, 1000000000));
    }

    LSF_ASSERT(sets.empty());

    // test batch insert
    for (set<uint32_t>::iterator iter = uint_group.begin(); iter != uint_group.end(); iter++) {
        LSF_ASSERT_ERR_ONLY(sets.Insert(TestNode(*iter, *iter)));
    }

    LSF_ASSERT(sets.full());

    // test batch find
    for (set<uint32_t>::iterator iter = uint_group.begin(); iter != uint_group.end(); iter++) {
        LSF_ASSERT_ERR_ONLY(*sets.Find(TestNode(*iter, *iter)) == TestNode(*iter, *iter));
    }

    // test batch erase
    for (set<uint32_t>::iterator iter = uint_group.begin(); iter != uint_group.end(); iter++) {
        LSF_ASSERT_ERR_ONLY(sets.Erase(TestNode(*iter, *iter)));
        LSF_ASSERT_ERR_ONLY(sets.begin() == sets.end() + 1);
    }

    LSF_ASSERT(sets.empty());

    LSF_ASSERT(SharedMem::Delete(SHM_KEY));
}

int main(int argc, char **argv) { LSF_TEST_ALL(argc, argv); }

// vim:ts=4:sw=4:et:ft=cpp:
