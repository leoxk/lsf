// File:        test1ing_ext.cpp
// Description: ---
// Notes:       ---
// Author:      leoxiang <leoxiang727@qq.com>
// Revision:    2012-05-19 by leoxiang

#include "lsf/basic/unit_test.hpp"
#include "lsf/util/string_ext.hpp"

using namespace std;
using namespace lsf::util;

LSF_TEST_CASE(remove_whitespace) {
    string test1 = "   test test  ";
    LSF_ASSERT(StringExt::RemoveHeadWhitespace(test1) == "test test  ");
    LSF_ASSERT(StringExt::RemoveTailWhitespace(test1) == "test test");

    string test2 = "   \n\n  ";
    LSF_ASSERT(StringExt::RemoveHeadWhitespace(test2).empty());

    string test3 = "   \n\n  ";
    LSF_ASSERT(StringExt::RemoveTailWhitespace(test3).empty());
}

LSF_TEST_CASE(basename_and_dirname_and_suffix) {
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

LSF_TEST_CASE(test_format) { LSF_ASSERT(StringExt::Format("%02d %04d", 12, 12) == "12 0012"); }

LSF_TEST_CASE(test_replace) {
    string s1("hello world");
    LSF_ASSERT(StringExt::Replace(s1, "hello", "fuck") == "fuck world");
    LSF_ASSERT(StringExt::Replace(s1, "hello", "hellohello") == "hellohello world");
    LSF_ASSERT(StringExt::Replace(s1, "h", "hh") == "hhello world");
    LSF_ASSERT(StringExt::Replace(s1, "hello ", "") == "world");

    LSF_ASSERT(StringExt::ReplaceInplace(s1, "hello ", "fuck ") == "fuck world");
    LSF_ASSERT(StringExt::ReplaceInplace(s1, "fuck", "hello") == "hello world");
    LSF_ASSERT(StringExt::ReplaceInplace(s1, "h", "hh") == "hhello world");
    LSF_ASSERT(StringExt::ReplaceInplace(s1, "hhello ", "") == "world");

    // cout << StringExt::Replace(s1, "h", "hh") << endl;
}

LSF_TEST_CASE(test_split) {
    LSF_ASSERT(StringExt::SplitGet("123/456/789", "/", 0) == "123");
    LSF_ASSERT(StringExt::SplitGet("123/456/789", "/", 1) == "456");
    LSF_ASSERT(StringExt::SplitGet("123/456/789", "/", 2) == "789");
    LSF_ASSERT(StringExt::SplitGet("123/456/789", "/", 3) == "");

    LSF_ASSERT(StringExt::SplitGet("123/456/789", '/', 0) == "123");
    LSF_ASSERT(StringExt::SplitGet("123/456/789", '/', 1) == "456");
    LSF_ASSERT(StringExt::SplitGet("123/456/789", '/', 2) == "789");
    LSF_ASSERT(StringExt::SplitGet("123/456/789", '/', 3) == "");

    LSF_ASSERT(StringExt::SplitGet("123/456/789", "/$#&^", 0) == "123");
    LSF_ASSERT(StringExt::SplitGet("123/456/789", "/$#&^", 1) == "456");
    LSF_ASSERT(StringExt::SplitGet("123/456/789", "/$#&^", 2) == "789");
    LSF_ASSERT(StringExt::SplitGet("123/456/789", "/$#&^", 3) == "");

    LSF_ASSERT(StringExt::SplitGet("123/456#789/", "/#", 0) == "123");
    LSF_ASSERT(StringExt::SplitGet("123/456#789/", "/#", 1) == "456");
    LSF_ASSERT(StringExt::SplitGet("123/456#789/", "/#", 2) == "789");
    LSF_ASSERT(StringExt::SplitGet("123/456#789/", "/#", 3) == "");

    LSF_ASSERT(StringExt::SplitGet("/123/456/789", "/", 0) == "");
    LSF_ASSERT(StringExt::SplitGet("/123/456/789", "/", 1) == "123");
    LSF_ASSERT(StringExt::SplitGet("/123/456/789", "/", 2) == "456");
    LSF_ASSERT(StringExt::SplitGet("/123/456/789", "/", 3) == "789");
    LSF_ASSERT(StringExt::SplitGet("/123/456/789", "/", 4) == "");

    LSF_ASSERT(StringExt::SplitGet("/123#456$789/", "/#$", 0) == "");
    LSF_ASSERT(StringExt::SplitGet("/123#456$789/", "/#$", 1) == "123");
    LSF_ASSERT(StringExt::SplitGet("/123#456$789/", "/#$", 2) == "456");
    LSF_ASSERT(StringExt::SplitGet("/123#456$789/", "/#$", 3) == "789");
    LSF_ASSERT(StringExt::SplitGet("/123#456$789/", "/#$", 4) == "");
}

LSF_TEST_CASE(test_find_and_trim_block) {
    string s("  [12312]111");
    LSF_ASSERT(StringExt::FindAndTrimBlock(s, ')', ']') == "  [12312]111");
    LSF_ASSERT(StringExt::FindAndTrimBlock(s, ')', ']', true) == "  [12312]111");
    s = "[[[[";
    LSF_ASSERT(StringExt::FindAndTrimBlock(s, '[', "[]", true) == "[[[[");
    LSF_ASSERT(StringExt::FindAndTrimBlock(s, '[', "[]") == "[[");
    s = "[[[[a321231";
    LSF_ASSERT(StringExt::FindAndTrimBlock(s, '[', "]", true) == "[[[[a321231");
    LSF_ASSERT(StringExt::FindAndTrimBlock(s, '[', "]") == "[[[[a321231");
    s = "[[[[a321231))";
    LSF_ASSERT(StringExt::FindAndTrimBlock(s, '[', ")", true) == "");
    s = "[[[[a321231))";
    LSF_ASSERT(StringExt::FindAndTrimBlock(s, '[', ")") == ")");
}

LSF_TEST_CASE(test_hex_string) {
    uint8_t content[] = {0x16, 0x34, 0xa8, 0xc0, 0x1f, 0x12, 0x12, 0x94, 0x16, 0xb8, 0xa8,
                         0x12, 0x12, 0xcc, 0xc1, 0xba, 0x16, 0x82, 0x00, 0xb7, 0x94, 0xaa,
                         0x94, 0xa1, 0x16, 0xaa, 0xb7, 0xb7, 0x12, 0xaa, 0xba, 0x94};

    LSF_ASSERT(StringExt::BinToHexString(content, sizeof(content)) ==
               "1634a8c01f12129416b8a81212ccc1ba168200b794aa94a116aab7b712aaba94");
    LSF_ASSERT(StringExt::BinToHexString(StringExt::HexStringToBin(StringExt::BinToHexString(
                   content, sizeof(content)))) == "1634a8c01f12129416b8a81212ccc1ba168200b794aa94a116aab7b712aaba94");

    LSF_ASSERT(StringExt::BinToString(content, sizeof(content)) == ".4..............................");

    std::string content_bin = StringExt::HexStringToBin(StringExt::BinToHexString(content, sizeof(content)));
    LSF_ASSERT(content_bin.size() == sizeof(content));
    LSF_ASSERT(content_bin.compare(0, std::string::npos, (char*)content, sizeof(content)) == 0);
}

LSF_TEST_CASE(test_utf_8) {
    LSF_ASSERT(StringExt::Utf8Length("this is utf8") == 12);
    LSF_ASSERT(StringExt::Utf8Length("全部是中文") == 5);
    LSF_ASSERT(StringExt::Utf8Length("中文english混合test") == 15);
}

int main(int argc, char** argv) { LSF_TEST_ALL(argc, argv); }

// vim:ts=4:sw=4:et:ft=cpp:
