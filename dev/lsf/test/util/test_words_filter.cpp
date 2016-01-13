// File:        test_words_filter.cpp
// Description: ---
// Notes:       ---
// Author:      leoxiang <leoxiang727@qq.com>
// Revision:    2015-09-15 by leoxiang

#include "lsf/basic/unit_test.hpp"
#include "lsf/util/words_filter.hpp"

using namespace lsf::util;

LSF_TEST_CASE(test_words_filter) {
    WordsFilter::Instance()->AddWord("fuck");
    WordsFilter::Instance()->AddWord("操你妈");

    LSF_ASSERT(WordsFilter::Instance()->CheckHasWord("我想fuck"));
    LSF_ASSERT(WordsFilter::Instance()->CheckHasWord("我想fuck啊啊啊"));
    LSF_ASSERT(WordsFilter::Instance()->CheckHasWord("我想fuck操你妈啊"));
    LSF_ASSERT(WordsFilter::Instance()->CheckHasWord("fuck"));
    LSF_ASSERT(WordsFilter::Instance()->CheckHasWord("操你妈"));
    LSF_ASSERT(WordsFilter::Instance()->CheckHasWord("操你妈逼"));
    LSF_ASSERT(WordsFilter::Instance()->CheckHasWord("我操你妈"));

    LSF_ASSERT(WordsFilter::Instance()->Substitue("我想fuck")         == "我想****");
    LSF_ASSERT(WordsFilter::Instance()->Substitue("我想fuck啊啊啊")   == "我想****啊啊啊");
    LSF_ASSERT(WordsFilter::Instance()->Substitue("我想fuck操你妈啊") == "我想*******啊");
    LSF_ASSERT(WordsFilter::Instance()->Substitue("fuck")             == "****");
    LSF_ASSERT(WordsFilter::Instance()->Substitue("操你妈")           == "***");
    LSF_ASSERT(WordsFilter::Instance()->Substitue("操你妈逼")         == "***逼");
    LSF_ASSERT(WordsFilter::Instance()->Substitue("我操你妈")         == "我***");
}

int main(int argc, char **argv) { LSF_TEST_ALL(argc, argv); }

// vim:ts=4:sw=4:et:ft=cpp:
