// File:        test_basic_container.cpp
// Description: ---
// Notes:       ---
// Author:      leoxiang <xiangkun@ximigame.com>
// Revision:    2014-10-08 by leoxiang
//
// Copyright (C) 1998-2014 XiMi. All Rights Reserved.
// This is unpublished proprietary source code of XiMi Ltd. The copyright
// notice above does not evidence any actual or intended publication of such
// source code. UNAUTHORIZED DISTRIBUTION, ADAPTATION OR USE MAY BE SUBJECT
// TO CIVIL AND CRIMINAL PENALTIES.

#include "lsf/basic/unit_test.hpp"
#include "lsf/container/list.hpp"
#include "node.hpp"

using namespace std;
using namespace lsf::container;

#define SHM_KEY  0x082157ff
#define QUEUE_SIZE 1024

LSF_TEST_CASE(bind_to_new_mem)
{
    if (SharedMem::IsShmExist(SHM_KEY)) LSF_ASSERT(SharedMem::Delete(SHM_KEY));
    LSF_ASSERT(SharedMem::Create(SHM_KEY, List<TestNode, SharedMem>::CalcByteSize(QUEUE_SIZE)));

    List<TestNode, SharedMem> list;

    LSF_ASSERT(list.BindAndInitStorage(SharedMem(SHM_KEY)));
    LSF_ASSERT(list.IsBindStorage());
    LSF_ASSERT(list.MaxSize() == QUEUE_SIZE);
    LSF_ASSERT(list.Size() == 0);
    LSF_ASSERT(list.IsEmpty());
    LSF_ASSERT(!list.IsFull());
    LSF_ASSERT(list.Size() == 0);

    LSF_ASSERT(list.PushBack(TestNode()));
    LSF_ASSERT(list.PushBack(TestNode()));
    LSF_ASSERT(list.PushBack(TestNode()));
    LSF_ASSERT(list.Size() == 3);
}

LSF_TEST_CASE(recovery_from_exist_mem)
{
    List<TestNode, SharedMem> list;

    LSF_ASSERT(list.BindAndRecoverStorage(SharedMem(SHM_KEY)));
    LSF_ASSERT(list.IsBindStorage());

    LSF_ASSERT(list.MaxSize() == QUEUE_SIZE);
    LSF_ASSERT(list.Size() == 3);
    LSF_ASSERT(list.ElemByteSize() == sizeof(TestNode));
}

LSF_TEST_CASE(recovery_from_new_type)
{
    List<TestNode1, SharedMem> list;

    LSF_ASSERT(!list.BindAndRecoverStorage(SharedMem(SHM_KEY)));
    LSF_ASSERT(!list.IsBindStorage());
    std::cout << list.ErrString() << std::endl;

}

int main(int argc, char **argv)
{
	LSF_TEST_ALL();
}

// vim:ts=4:sw=4:et:
