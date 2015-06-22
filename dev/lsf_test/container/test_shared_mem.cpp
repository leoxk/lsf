// File:        test_shared_mem.cpp
// Description: ---
// Notes:       ---
// Author:      leoxiang <leoxiang727@qq.com>
// Revision:    2012-05-07 by leoxiang

#include "lsf/basic/unit_test.hpp"
#include "lsf/container/shared_mem.hpp"

using namespace std;
using namespace lsf::container;

#define SHM_KEY 0x082157ff
#define SHM_SIZE 10 * 1024

LSF_TEST_CASE(create_and_delete) {
    if (SharedMem::IsShmExist(SHM_KEY)) SharedMem::Delete(SHM_KEY);

    LSF_ASSERT(SharedMem::Create(SHM_KEY, SHM_SIZE));
    LSF_ASSERT(SharedMem::QueryAttachedNum(SHM_KEY) == 0);
    LSF_ASSERT(SharedMem::IsShmExist(SHM_KEY));
    LSF_ASSERT(!SharedMem::IsShmExist(SHM_KEY + 123));
    LSF_ASSERT(SharedMem::QueryShmSize(SHM_KEY) == SHM_SIZE);

    LSF_ASSERT(SharedMem::Delete(SHM_KEY));
    LSF_ASSERT(!SharedMem::IsShmExist(SHM_KEY));
    LSF_ASSERT(SharedMem::QueryShmSize(SHM_KEY) == 0);
}

LSF_TEST_CASE(attach_and_detach) {
    LSF_ASSERT(SharedMem::Create(SHM_KEY, SHM_SIZE));

    SharedMem shm;
    SharedMem shm1;

    // create and attach
    LSF_ASSERT(shm.Attach(SHM_KEY));
    LSF_ASSERT(shm.IsAttached());
    LSF_ASSERT(shm.GetPtr() != nullptr);
    LSF_ASSERT(shm.GetAttachedNum() == 1);
    LSF_ASSERT(shm.GetSize() == SHM_SIZE);

    // detach
    LSF_ASSERT(shm.Detach());
    LSF_ASSERT(!shm.IsAttached());
    LSF_ASSERT(shm.GetPtr() == nullptr);
    LSF_ASSERT(shm.GetAttachedNum() == 0);
    LSF_ASSERT(shm.GetSize() == 0);

    // multi attached
    LSF_ASSERT(shm.Attach(SHM_KEY));
    LSF_ASSERT(!shm1.IsAttached());
    LSF_ASSERT(shm.GetAttachedNum() == 1);

    shm1 = shm;
    LSF_ASSERT(shm1.IsAttached());
    LSF_ASSERT(shm.GetAttachedNum() == 2);

    {
        SharedMem shm2 = shm;
        LSF_ASSERT(shm2.IsAttached());
        LSF_ASSERT(shm.GetAttachedNum() == 3);
    }
    LSF_ASSERT(shm.GetAttachedNum() == 2);

    // delete
    LSF_ASSERT(SharedMem::Delete(SHM_KEY));
    LSF_ASSERT(SharedMem::QueryAttachedNum(SHM_KEY) == 0);
}

LSF_TEST_CASE(lock_and_unlock) {
    LSF_ASSERT(SharedMem::Create(SHM_KEY, SHM_SIZE));
    LSF_ASSERT(SharedMem::Lock(SHM_KEY));
    LSF_ASSERT(SharedMem::UnLock(SHM_KEY));
    LSF_ASSERT(SharedMem::Delete(SHM_KEY));
}

LSF_TEST_CASE(test_readonly_attach) {
    SharedMem shm;
    LSF_ASSERT(SharedMem::Create(SHM_KEY, SHM_SIZE));

    // assign in read-write attach
    LSF_ASSERT(shm.Attach(SHM_KEY));
    *((size_t *)shm.GetPtr()) = 123;
    LSF_ASSERT(shm.Detach());

    // assign in read-only attach
    LSF_ASSERT(shm.Attach(SHM_KEY, SHM_RDONLY));
    LSF_ASSERT(*(size_t *)shm.GetPtr() == 123);
    *((size_t *)shm.GetPtr()) = 456;
    LSF_ASSERT(shm.Detach());

    LSF_ASSERT(shm.Attach(SHM_KEY));
    LSF_ASSERT(*(size_t *)shm.GetPtr() == 456);
    LSF_ASSERT(shm.Detach());
}

int main(int argc, char **argv) { LSF_TEST_ALL(argc, argv); }

// vim:ts=4:sw=4:et:ft=cpp:
