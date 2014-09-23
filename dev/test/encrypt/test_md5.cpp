// File:        test_md5.cpp
// Description: ---
// Notes:       ---
// Author:      leoxiang <leoxiang@tencent.com>
// Revision:    2012-06-01 by leoxiang

#include "lsf/basic/unit_test.hpp"
#include "lsf/encrypt/md5.hpp"
#include "lsf/basic/buffer.hpp"

using namespace std;
using namespace lsf::encrypt;
using namespace lsf::basic;

const char content[] = "leoxiang";

LSF_TEST_CASE(test)
{
    char enc_buf[16];

    Md5HashBuffer(enc_buf, content, strlen(content));

    LSF_ASSERT(strlen(content) == 8);
    LSF_ASSERT(BinToHexString(enc_buf, sizeof(enc_buf)) == "637efe4601a183fb76b3b013101f7758");
}

int main(int argc, char **argv)
{
	LSF_TEST_ALL();
}

// vim:ts=4:sw=4:et:ft=cpp:
