// File:        test_shared_ptr.cpp
// Description: ---
// Notes:       ---
// Author:      leoxiang <leoxiang727@qq.com>
// Revision:    2011-12-09 by leoxiang

#include <string>
#include "lsf/basic/unit_test.hpp"
#include "lsf/util/shared_ptr.hpp"

using namespace std;
using namespace lsf::util;

LSF_TEST_CASE(test_constructor)
{
    SharedPtr<string> sp(new string("test1"));
    LSF_ASSERT(sp.IsUnique());
    LSF_ASSERT(sp.GetUseCount() == 1);
    LSF_ASSERT(*sp == "test1");
    LSF_ASSERT(sp->size() == 5);

    // test constructor
    SharedPtr<string> sp2(sp);
    LSF_ASSERT(sp2.GetUseCount() == 2);
    LSF_ASSERT(*sp2 == "test1");
    LSF_ASSERT(sp2->size() == 5);

    // test copy constructor
    SharedPtr<string> sp3 = sp;
    LSF_ASSERT(sp2.GetUseCount() == 3);
    LSF_ASSERT(*sp2 == "test1");
    LSF_ASSERT(sp2->size() == 5);
}

LSF_TEST_CASE(test_copy)
{
    SharedPtr<string> sp(new string("test2"));
    SharedPtr<string> sp1;
    SharedPtr<string> sp2;
    SharedPtr<string> sp3;
    SharedPtr<string> sp4;
    SharedPtr<string> sp5;

    sp1 = sp;
    sp2 = sp1;
    sp3 = sp2;
    sp4 = sp3;
    sp5 = sp4;

    LSF_ASSERT(sp1.GetUseCount() == 6);
    LSF_ASSERT(sp2.GetUseCount() == 6);
    LSF_ASSERT(sp3.GetUseCount() == 6);
    LSF_ASSERT(sp4.GetUseCount() == 6);
    LSF_ASSERT(sp5.GetUseCount() == 6);

    LSF_ASSERT(*sp1 == "test2");
    LSF_ASSERT(*sp2 == "test2");
    LSF_ASSERT(*sp3 == "test2");
    LSF_ASSERT(*sp4 == "test2");
    LSF_ASSERT(*sp5 == "test2");
}

class NewString : public std::string {
public:
    NewString(char const *ptr) : std::string(ptr) { }
};

LSF_TEST_CASE(test)
{
    SharedPtr<NewString> sp;
    sp = new NewString("test");

    vector<SharedPtr<string> > vsp;

    vsp.push_back(sp);
    vsp.push_back(SharedPtr<NewString>(sp));
    vsp.push_back(SharedPtr<NewString>(sp));
    vsp.push_back(SharedPtr<NewString>(sp));
    vsp.push_back(SharedPtr<NewString>(sp));

    LSF_ASSERT(sp.GetUseCount() == 6);
    LSF_ASSERT(*vsp[0] == "test");
    LSF_ASSERT(*vsp[1] == "test");
    LSF_ASSERT(*vsp[2] == "test");
    LSF_ASSERT(*vsp[3] == "test");
    LSF_ASSERT(*vsp[4] == "test");
}

int main(int argc, char** argv)
{
    LSF_TEST_ALL();
}

// vim:ts=4:sw=4:et:ft=cpp:
