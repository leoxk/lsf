// File:        test1ing_ext.cpp
// Description: ---
// Notes:       ---
// Author:      leoxiang <leoxiang727@qq.com>
// Revision:    2012-05-19 by leoxiang

#include "lsf/basic/unit_test.hpp"
#include "lsf/basic/string_ext.hpp"

using namespace std;
using namespace lsf::basic;

LSF_TEST_CASE(remove_whitespace)
{
    string test1 = "   test test  ";
    LSF_ASSERT(StringExt::RemoveHeadWhitespace(test1) == "test test  ");
    LSF_ASSERT(StringExt::RemoveTailWhitespace(test1) == "test test");

    string test2 = "   \n\n  ";
    LSF_ASSERT(StringExt::RemoveHeadWhitespace(test2).empty());

    string test3 = "   \n\n  ";
    LSF_ASSERT(StringExt::RemoveTailWhitespace(test3).empty());
}

LSF_TEST_CASE(basename_and_dirname_and_suffix)
{
    // GetDirName
    LSF_ASSERT(StringExt::GetDirName("/dirname/basename.suffix") == "/dirname");
    LSF_ASSERT(StringExt::GetDirName("/dirname/basename.suffix/") == "/dirname");
    LSF_ASSERT(StringExt::GetDirName("/dirname/basename.suffix///") == "/dirname");
    LSF_ASSERT(StringExt::GetDirName("  /dirname/basename.suffix///  ") == "/dirname");

    LSF_ASSERT(StringExt::GetDirName("/dirname") == "/");
    LSF_ASSERT(StringExt::GetDirName("/dirname/") == "/");
    LSF_ASSERT(StringExt::GetDirName("/dirname///") == "/");
    LSF_ASSERT(StringExt::GetDirName("  /dirname///  ") == "/");

    LSF_ASSERT(StringExt::GetDirName("basename.suffix") == ".");
    LSF_ASSERT(StringExt::GetDirName("basename.suffix/") == ".");
    LSF_ASSERT(StringExt::GetDirName("basename.suffix///") == ".");
    LSF_ASSERT(StringExt::GetDirName("  basename.suffix///  ") == ".");

    // GetBaseName
    LSF_ASSERT(StringExt::GetBaseName("/dirname/basename.suffix") == "basename.suffix");
    LSF_ASSERT(StringExt::GetBaseName("/dirname/basename.suffix/") == "basename.suffix");
    LSF_ASSERT(StringExt::GetBaseName("/dirname/basename.suffix///") == "basename.suffix");
    LSF_ASSERT(StringExt::GetBaseName("  /dirname/basename.suffix///  ") == "basename.suffix");

    LSF_ASSERT(StringExt::GetBaseName("basename.suffix") == "basename.suffix");
    LSF_ASSERT(StringExt::GetBaseName("basename.suffix/") == "basename.suffix");
    LSF_ASSERT(StringExt::GetBaseName("basename.suffix///") == "basename.suffix");
    LSF_ASSERT(StringExt::GetBaseName("  basename.suffix///  ") == "basename.suffix");

    // GetSuffix
    LSF_ASSERT(StringExt::GetSuffix("/dirname/basename.suffix") == "suffix");
    LSF_ASSERT(StringExt::GetSuffix("/dirname/basename.suffix/") == "suffix");
    LSF_ASSERT(StringExt::GetSuffix("/dirname/basename.suffix///") == "suffix");
    LSF_ASSERT(StringExt::GetSuffix("  /dirname/basename.suffix///  ") == "suffix");

    LSF_ASSERT(StringExt::GetSuffix("/dirname/.suffix") == "suffix");
    LSF_ASSERT(StringExt::GetSuffix("/dirname/.suffix/") == "suffix");
    LSF_ASSERT(StringExt::GetSuffix("/dirname/.suffix///") == "suffix");
    LSF_ASSERT(StringExt::GetSuffix("  /dirname/.suffix///  ") == "suffix");

    LSF_ASSERT(StringExt::GetSuffix("/dirname/.") == "");
    LSF_ASSERT(StringExt::GetSuffix("/dirname/./") == "");
    LSF_ASSERT(StringExt::GetSuffix("/dirname/.///") == "");
    LSF_ASSERT(StringExt::GetSuffix("  /dirname/.///  ") == "");
}

LSF_TEST_CASE(test_format)
{
    LSF_ASSERT(StringExt::Format("%02d %04d", 12, 12) == "12 0012");
}

LSF_TEST_CASE(test_replace)
{
    string s1("hello world");
    LSF_ASSERT(StringExt::Replace(s1, "hello", "fuck") == "fuck world");
    LSF_ASSERT(StringExt::Replace(s1, "hello", "hellohello") == "hellohello world");
    LSF_ASSERT(StringExt::Replace(s1, "h", "hh") == "hhello world");
    LSF_ASSERT(StringExt::Replace(s1, "hello ", "") == "world");

    LSF_ASSERT(StringExt::ReplaceInplace(s1, "hello ", "fuck ") == "fuck world");
    LSF_ASSERT(StringExt::ReplaceInplace(s1, "fuck",  "hello") == "hello world");
    LSF_ASSERT(StringExt::ReplaceInplace(s1, "h", "hh") == "hhello world");
    LSF_ASSERT(StringExt::ReplaceInplace(s1, "hhello ", "") == "world");

    //cout << StringExt::Replace(s1, "h", "hh") << endl;
}

LSF_TEST_CASE(test_split)
{
    LSF_ASSERT(StringExt::SplitAndGet("123/456/789", "/", 0) == "123");
    LSF_ASSERT(StringExt::SplitAndGet("123/456/789", "/", 1) == "456");
    LSF_ASSERT(StringExt::SplitAndGet("123/456/789", "/", 2) == "789");
    LSF_ASSERT(StringExt::SplitAndGet("123/456/789", "/", 3) == "");

    LSF_ASSERT(StringExt::SplitAndGet("123/456/789", '/', 0) == "123");
    LSF_ASSERT(StringExt::SplitAndGet("123/456/789", '/', 1) == "456");
    LSF_ASSERT(StringExt::SplitAndGet("123/456/789", '/', 2) == "789");
    LSF_ASSERT(StringExt::SplitAndGet("123/456/789", '/', 3) == "");

    LSF_ASSERT(StringExt::SplitAndGet("123/456/789", "/$#&^", 0) == "123");
    LSF_ASSERT(StringExt::SplitAndGet("123/456/789", "/$#&^", 1) == "456");
    LSF_ASSERT(StringExt::SplitAndGet("123/456/789", "/$#&^", 2) == "789");
    LSF_ASSERT(StringExt::SplitAndGet("123/456/789", "/$#&^", 3) == "");

    LSF_ASSERT(StringExt::SplitAndGet("123/456#789/", "/#", 0) == "123");
    LSF_ASSERT(StringExt::SplitAndGet("123/456#789/", "/#", 1) == "456");
    LSF_ASSERT(StringExt::SplitAndGet("123/456#789/", "/#", 2) == "789");
    LSF_ASSERT(StringExt::SplitAndGet("123/456#789/", "/#", 3) == "");

    LSF_ASSERT(StringExt::SplitAndGet("/123/456/789", "/", 0) == "");
    LSF_ASSERT(StringExt::SplitAndGet("/123/456/789", "/", 1) == "123");
    LSF_ASSERT(StringExt::SplitAndGet("/123/456/789", "/", 2) == "456");
    LSF_ASSERT(StringExt::SplitAndGet("/123/456/789", "/", 3) == "789");
    LSF_ASSERT(StringExt::SplitAndGet("/123/456/789", "/", 4) == "");

    LSF_ASSERT(StringExt::SplitAndGet("/123#456$789/", "/#$", 0) == "");
    LSF_ASSERT(StringExt::SplitAndGet("/123#456$789/", "/#$", 1) == "123");
    LSF_ASSERT(StringExt::SplitAndGet("/123#456$789/", "/#$", 2) == "456");
    LSF_ASSERT(StringExt::SplitAndGet("/123#456$789/", "/#$", 3) == "789");
    LSF_ASSERT(StringExt::SplitAndGet("/123#456$789/", "/#$", 4) == "");
}

int main(int argc, char** argv)
{
    LSF_TEST_ALL();
}

// vim:ts=4:sw=4:et:ft=cpp:
