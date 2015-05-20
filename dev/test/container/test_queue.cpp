// File:        test_queue.cpp
// Description: ---
// Notes:       ---
// Author:      leoxiang <leoxiang727@qq.com>
// Revision:    2012-05-10 by leoxiang

#include "lsf/basic/unit_test.hpp"
#include "lsf/container/queue.hpp"
#include "node.hpp"

using namespace std;
using namespace lsf::container;

#define SHM_KEY  0x082157ff
#define SHM_SIZE 1024

LSF_TEST_CASE(bind_to_new_mem)
{
    if (SharedMem::IsShmExist(SHM_KEY)) LSF_ASSERT(SharedMem::Delete(SHM_KEY));
    LSF_ASSERT(SharedMem::Create(SHM_KEY, Queue<TestNode, SharedMem>::CalcByteSize(SHM_SIZE)));

    Queue<TestNode, SharedMem> queue;

    LSF_ASSERT(queue.BindAndInitStorage(SharedMem(SHM_KEY)));

    LSF_ASSERT(queue.MaxSize() == SHM_SIZE);
    LSF_ASSERT(queue.IsBindStorage());
    LSF_ASSERT(queue.MaxSize() == SHM_SIZE);
    LSF_ASSERT(queue.Size() == 0);
    LSF_ASSERT(queue.IsEmpty());
    LSF_ASSERT(!queue.IsFull());

    LSF_ASSERT(queue.PushBack(TestNode()));
    LSF_ASSERT(queue.PushBack(TestNode()));
    LSF_ASSERT(queue.PushBack(TestNode()));
    LSF_ASSERT(queue.Size() == 3);
}

LSF_TEST_CASE(recovery_from_exist_mem)
{
    Queue<TestNode, SharedMem> queue;

    LSF_ASSERT(queue.BindAndRecoverStorage(SharedMem(SHM_KEY)));
    LSF_ASSERT(queue.IsBindStorage());

    LSF_ASSERT(queue.MaxSize() == SHM_SIZE);
    LSF_ASSERT(queue.Size() == 3);

    LSF_ASSERT(SharedMem::Delete(SHM_KEY));
}

LSF_TEST_CASE(queue_common_funcs)
{
    // use shorter size type
    Queue<TestNode, SharedMem, uint16_t> queue;

    if (SharedMem::IsShmExist(SHM_KEY)) LSF_ASSERT(SharedMem::Delete(SHM_KEY));
    LSF_ASSERT(SharedMem::Create(SHM_KEY, queue.CalcByteSize(5)));
    LSF_ASSERT(queue.BindAndInitStorage(SharedMem(SHM_KEY)));
    LSF_ASSERT(queue.MaxSize() == 5);
    cout << queue.MaxSize() << endl;
    LSF_ASSERT(queue.IsBindStorage());

    // common func
    LSF_ASSERT(queue.IsEmpty());
    LSF_ASSERT(queue.PushBack(TestNode(1, 1)));
    LSF_ASSERT(queue.PushBack(TestNode(2, 2)));
    LSF_ASSERT(queue.PushBack(TestNode(3, 3)));
    LSF_ASSERT(queue.PushBack(TestNode(4, 4)));
    LSF_ASSERT(queue.PushBack(TestNode(5, 5)));
    LSF_ASSERT(!queue.PushBack(TestNode(6, 6)));
    LSF_ASSERT(queue.IsFull());
    LSF_ASSERT(queue.Size() == 5);

    LSF_ASSERT(*queue.GetFront() == TestNode(1, 1));
    LSF_ASSERT(queue.PopFront());
    LSF_ASSERT(*queue.GetFront() == TestNode(2, 2));
    LSF_ASSERT(queue.PopFront());
    LSF_ASSERT(queue.PopFront());
    LSF_ASSERT(queue.PopFront());
    LSF_ASSERT(*queue.GetFront() == TestNode(5, 5));
    LSF_ASSERT(queue.Size() == 1);
    LSF_ASSERT(queue.PopFront());
    LSF_ASSERT(queue.IsEmpty());

    LSF_ASSERT(SharedMem::Delete(SHM_KEY));
}

LSF_TEST_CASE(test_push_and_pop_from_front_and_back)
{
    Queue<TestNode, SharedMem> queue;

    if (SharedMem::IsShmExist(SHM_KEY)) LSF_ASSERT(SharedMem::Delete(SHM_KEY));
    LSF_ASSERT(SharedMem::Create(SHM_KEY, Queue<TestNode, SharedMem>::CalcByteSize(10)));
    LSF_ASSERT(queue.BindAndInitStorage(SharedMem(SHM_KEY)));
    LSF_ASSERT(queue.MaxSize() == 10);

    LSF_ASSERT(queue.PushFront(TestNode(1, 1)));
    LSF_ASSERT(queue.GetFront() == queue.GetBack());
    LSF_ASSERT(queue.PopFront());
    LSF_ASSERT(queue.PushFront(TestNode(1, 1)));
    LSF_ASSERT(queue.PopBack());

    LSF_ASSERT(queue.PushFront(TestNode(1, 1)));
    LSF_ASSERT(queue.PushFront(TestNode(2, 2)));
    LSF_ASSERT(queue.PushFront(TestNode(3, 3)));
    LSF_ASSERT(queue.PushFront(TestNode(4, 4)));
    LSF_ASSERT(queue.PushFront(TestNode(5, 5)));
    LSF_ASSERT(queue.PushFront(TestNode(6, 6)));
    LSF_ASSERT(*(queue.GetFront()) == TestNode(6, 6));
    LSF_ASSERT(*(queue.GetBack())  == TestNode(1, 1));
    LSF_ASSERT(queue.PopFront());
    LSF_ASSERT(*(queue.GetFront()) == TestNode(5, 5));
    LSF_ASSERT(queue.PopBack());
    LSF_ASSERT(*(queue.GetBack())  == TestNode(2, 2));
    LSF_ASSERT(queue.PopFront());
    LSF_ASSERT(queue.PopBack());
    LSF_ASSERT(queue.PopBack());
    LSF_ASSERT(queue.PopFront());
    LSF_ASSERT(queue.IsEmpty());

    LSF_ASSERT(SharedMem::Delete(SHM_KEY));
}

LSF_TEST_CASE(test_iterator)
{
    Queue<TestNode, SharedMem> queue;
    if (SharedMem::IsShmExist(SHM_KEY)) LSF_ASSERT(SharedMem::Delete(SHM_KEY));
    LSF_ASSERT(SharedMem::Create(SHM_KEY, Queue<TestNode, SharedMem>::CalcByteSize(10)));
    LSF_ASSERT(queue.BindAndInitStorage(SharedMem(SHM_KEY)));
    LSF_ASSERT(queue.MaxSize() == 10);

    LSF_ASSERT(queue.PushBack(TestNode(1, 1)));
    LSF_ASSERT(queue.PushBack(TestNode(2, 2)));
    LSF_ASSERT(queue.PushBack(TestNode(3, 3)));
    LSF_ASSERT(queue.PushBack(TestNode(4, 4)));
    LSF_ASSERT(queue.PushBack(TestNode(5, 5)));
    LSF_ASSERT(queue.PushBack(TestNode(6, 6)));
    LSF_ASSERT(queue.PushBack(TestNode(7, 7)));
    LSF_ASSERT(queue.PushBack(TestNode(8, 8)));
    LSF_ASSERT(queue.PushBack(TestNode(9, 9)));
    LSF_ASSERT(queue.PushBack(TestNode(0, 0)));
    LSF_ASSERT(queue.PopFront());
    LSF_ASSERT(queue.PopFront());
    LSF_ASSERT(queue.PopFront());
    LSF_ASSERT(queue.PushBack(TestNode(1, 1)));
    LSF_ASSERT(queue.PushBack(TestNode(2, 2)));
    LSF_ASSERT(queue.PushBack(TestNode(3, 3)));

    // test iterator
    Queue<TestNode, SharedMem>::iterator iter_begin = queue.Begin();
    Queue<TestNode, SharedMem>::iterator iter = iter_begin;
    Queue<TestNode, SharedMem>::iterator iter_end   = queue.End();
    LSF_ASSERT(iter_begin != iter_end);
    LSF_ASSERT(iter == iter_begin);
    LSF_ASSERT(*(iter++) == TestNode(4, 4));
    LSF_ASSERT(*(iter++) == TestNode(5, 5));
    LSF_ASSERT(*(iter++) == TestNode(6, 6));
    LSF_ASSERT(*(iter++) == TestNode(7, 7));
    LSF_ASSERT(*(iter++) == TestNode(8, 8));
    LSF_ASSERT(*(iter++) == TestNode(9, 9));
    LSF_ASSERT(*(iter++) == TestNode(0, 0));
    LSF_ASSERT(*(iter++) == TestNode(1, 1));
    LSF_ASSERT(*(iter++) == TestNode(2, 2));
    LSF_ASSERT(*(iter++) == TestNode(3, 3));
    LSF_ASSERT(queue.Size() == 10);
    LSF_ASSERT(iter == iter_end);

    LSF_ASSERT(iter == iter_end);
    LSF_ASSERT(*(--iter) == TestNode(3, 3));
    LSF_ASSERT(*(--iter) == TestNode(2, 2));
    LSF_ASSERT(*(--iter) == TestNode(1, 1));
    LSF_ASSERT(*(--iter) == TestNode(0, 0));
    LSF_ASSERT(*(--iter) == TestNode(9, 9));
    LSF_ASSERT(*(--iter) == TestNode(8, 8));
    LSF_ASSERT(*(--iter) == TestNode(7, 7));
    LSF_ASSERT(*(--iter) == TestNode(6, 6));
    LSF_ASSERT(*(--iter) == TestNode(5, 5));
    LSF_ASSERT(*(--iter) == TestNode(4, 4));
    LSF_ASSERT(queue.Size() == 10);
    LSF_ASSERT(iter == iter_begin);

    // test reverse iterator
    Queue<TestNode, SharedMem>::reverse_iterator iter_rbegin = queue.RBegin();
    Queue<TestNode, SharedMem>::reverse_iterator iter_rend   = queue.REnd();
    Queue<TestNode, SharedMem>::reverse_iterator riter = iter_rbegin;
    LSF_ASSERT(iter_rbegin != iter_rend);
    LSF_ASSERT(riter == iter_rbegin);
    LSF_ASSERT(*(riter++) == TestNode(3, 3));
    LSF_ASSERT(*(riter++) == TestNode(2, 2));
    LSF_ASSERT(*(riter++) == TestNode(1, 1));
    LSF_ASSERT(*(riter++) == TestNode(0, 0));
    LSF_ASSERT(*(riter++) == TestNode(9, 9));
    LSF_ASSERT(*(riter++) == TestNode(8, 8));
    LSF_ASSERT(*(riter++) == TestNode(7, 7));
    LSF_ASSERT(*(riter++) == TestNode(6, 6));
    LSF_ASSERT(*(riter++) == TestNode(5, 5));
    LSF_ASSERT(*(riter++) == TestNode(4, 4));
    LSF_ASSERT(queue.Size() == 10);
    LSF_ASSERT(riter == iter_rend);

    // test erase
    while (queue.Size() != 0)
    {
        LSF_ASSERT(queue.PopFront());
    }
    LSF_ASSERT(queue.IsEmpty());

    LSF_ASSERT(SharedMem::Delete(SHM_KEY));
}

int main(int argc, char **argv)
{
	LSF_TEST_ALL();
}

// vim:ts=4:sw=4:et:ft=cpp:
