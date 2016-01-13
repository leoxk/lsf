// File:        test_basic_container.cpp
// Description: ---
// Notes:       ---
// Author:      leoxiang <leoxiang727@qq.com>
// Revision:    2014-10-08 by leoxiang

#include "lsf/basic/unit_test.hpp"
#include "lsf/container/list.hpp"
#include "node.hpp"

using namespace std;
using namespace lsf::container;

#define SHM_KEY 0x082157ff
#define QUEUE_SIZE 1024

LSF_TEST_CASE(bind_to_new_mem) {
    if (SharedMem::IsShmExist(SHM_KEY)) LSF_ASSERT(SharedMem::Delete(SHM_KEY));
    LSF_ASSERT(SharedMem::Create(SHM_KEY, List<TestNode, SharedMem>::CalcByteSize(QUEUE_SIZE)));

    List<TestNode, SharedMem> list;

    LSF_ASSERT(list.BindAndInitStorage(SharedMem(SHM_KEY)));
    LSF_ASSERT(list.IsBindStorage());
    LSF_ASSERT(list.max_size() == QUEUE_SIZE);
    LSF_ASSERT(list.size() == 0);
    LSF_ASSERT(list.empty());
    LSF_ASSERT(!list.full());
    LSF_ASSERT(list.size() == 0);

    LSF_ASSERT(list.PushBack(TestNode()));
    LSF_ASSERT(list.PushBack(TestNode()));
    LSF_ASSERT(list.PushBack(TestNode()));
    LSF_ASSERT(list.size() == 3);
}

LSF_TEST_CASE(recovery_from_exist_mem) {
    List<TestNode, SharedMem> list;

    LSF_ASSERT(list.BindAndRecoverStorage(SharedMem(SHM_KEY)));
    LSF_ASSERT(list.IsBindStorage());

    LSF_ASSERT(list.max_size() == QUEUE_SIZE);
    LSF_ASSERT(list.size() == 3);
    LSF_ASSERT(list.ElemByteSize() == sizeof(TestNode));
}

LSF_TEST_CASE(recovery_from_new_type) {
    List<TestNode1, SharedMem> list;

    LSF_ASSERT(!list.BindAndRecoverStorage(SharedMem(SHM_KEY)));
    LSF_ASSERT(!list.IsBindStorage());
}

int main(int argc, char **argv) {
    LSF_TEST_ALL(argc, argv);
    SharedMem::Delete(SHM_KEY);
}

// vim:ts=4:sw=4:et:
