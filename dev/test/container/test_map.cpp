// File:        test_map.cpp
// Description: ---
// Notes:       ---
// Author:      leoxiang <leoxiang@tencent.com>
// Revision:    2012-11-28 by leoxiang
//
// Copyright (C) 1998-2011 Tencent. All Rights Reserved.
// This is unpublished proprietary source code of Tencent Ltd. The copyright
// notice above does not evidence any actual or intended publication of such
// source code. UNAUTHORIZED DISTRIBUTION, ADAPTATION OR USE MAY BE SUBJECT
// TO CIVIL AND CRIMINAL PENALTIES.

#include <string>
#include "lsf/basic/unit_test.hpp"
#include "lsf/container/map.hpp"
#include "lsf/container/heap_mem.hpp"
#include "node.hpp"

using namespace std;
using namespace lsf::container;

#define CACHE_SIZE 5

LSF_TEST_CASE(easy_test)
{
    Map<int, string, HeapMem>    maps;
    maps.BindAndInitStorage(HeapMem(maps.CalcByteSize(CACHE_SIZE)));
    LSF_ASSERT(maps.IsBindStorage());
    LSF_ASSERT(maps.IsEmpty());

    // test insert
    LSF_ASSERT(maps.Insert(1, "leo1"));
    LSF_ASSERT(maps.Insert(2, "leo2"));
    LSF_ASSERT(maps.Insert(3, "leo3"));
    LSF_ASSERT(maps.Insert(4, "leo4"));
    LSF_ASSERT(maps.Insert(5, "leo5"));
    LSF_ASSERT(maps.IsFull());
    
    // test find
    LSF_ASSERT(maps.Find(1)->value == "leo1");
    LSF_ASSERT(maps.Find(2)->value == "leo2");
    LSF_ASSERT(maps.Find(3)->value == "leo3");
    LSF_ASSERT(maps.Find(4)->value == "leo4");
    LSF_ASSERT(maps.Find(5)->value == "leo5");

    // test iterator
    LSF_ASSERT((maps.Begin() + 0)->value == "leo1");
    LSF_ASSERT((maps.Begin() + 1)->value == "leo2");
    LSF_ASSERT((maps.Begin() + 2)->value == "leo3");
    LSF_ASSERT((maps.Begin() + 3)->value == "leo4");
    LSF_ASSERT((maps.Begin() + 4)->value == "leo5");

    // test erase
    LSF_ASSERT(maps.Erase(1));
    LSF_ASSERT(maps.Erase(2));
    LSF_ASSERT(maps.Erase(3));
    LSF_ASSERT(maps.Erase(4));
    LSF_ASSERT(maps.Erase(5));
    LSF_ASSERT(maps.IsEmpty());
}

LSF_TEST_CASE(test_iter_erase)
{
    Map<int, string, HeapMem>    maps;
    maps.BindAndInitStorage(HeapMem(maps.CalcByteSize(CACHE_SIZE)));
    LSF_ASSERT(maps.IsBindStorage());
    LSF_ASSERT(maps.IsEmpty());

    // test insert
    LSF_ASSERT(maps.Insert(1, "leo1"));
    LSF_ASSERT(maps.Insert(2, "leo2"));
    LSF_ASSERT(maps.Insert(3, "leo3"));
    LSF_ASSERT(maps.Insert(4, "leo4"));
    LSF_ASSERT(maps.Insert(5, "leo5"));
    LSF_ASSERT(maps.IsFull());
    
    // erase
    Map<int, string>::iterator iter = maps.Begin();
    while (iter != maps.End())
    {
        Map<int, string>::iterator cur_iter = iter++;
        LSF_ASSERT(maps.Erase(cur_iter->key));
    }
    LSF_ASSERT(maps.IsEmpty());
}

int main(int argc, char **argv)
{
	LSF_TEST_ALL();
}

// vim:ts=4:sw=4:et:ft=cpp:
