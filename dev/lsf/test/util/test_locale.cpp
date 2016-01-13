// File:        test_locale.cpp
// Description: ---
// Notes:       ---
// Author:      leoxiang <leoxiang727@qq.com>
// Revision:    2015-08-13 by leoxiang

#include "lsf/basic/unit_test.hpp"
#include "lsf/util/locale.hpp"

using namespace lsf::util;

LSF_TEST_CASE(test_code_convert) {
    std::string input = "我们发觉大师级的理科均为电脑阿里纳打死了你现在看来擦";
    std::string gbk;
    std::string utf8;

    LSF_ASSERT(Locale::ConvertUtf8ToGbk(input, gbk));
    LSF_ASSERT(Locale::ConvertGbkToUtf8(gbk, utf8));
    LSF_ASSERT(utf8 == input);
}

int main(int argc, char **argv) { LSF_TEST_ALL(argc, argv); }

// vim:ts=4:sw=4:et:ft=cpp:
