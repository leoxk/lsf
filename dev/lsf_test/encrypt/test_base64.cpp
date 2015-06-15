// File:        test_base64.cpp
// Description: ---
// Notes:       ---
// Author:      leoxiang <leoxiang727@qq.com>
// Revision:    2012-05-29 by leoxiang

#include "lsf/basic/unit_test.hpp"
#include "lsf/encrypt/base64.hpp"

using namespace std;
using namespace lsf::encrypt;

const unsigned char content[32] = { 
    0x16, 0x34, 0xa8, 0xc0, 0x1f, 0x12, 0x12, 0x94,
    0x16, 0xb8, 0xa8, 0x12, 0x12, 0xcc, 0xc1, 0xba,
    0x16, 0x82, 0x12, 0xb7, 0x94, 0xaa, 0x94, 0xa1,
    0x16, 0xaa, 0xb7, 0xb7, 0x12, 0xaa, 0xba, 0x94
};

LSF_TEST_CASE(test)
{
    char enc_buf[64];
    char dec_buf[64];
    size_t enc_len = sizeof(enc_buf);
    size_t dec_len = sizeof(dec_buf);

    LSF_ASSERT(Base64Encode(enc_buf, &enc_len, content, sizeof(content)));
    cout << "enc len: " << enc_len << endl;
    cout << "enc content: " << enc_buf << endl;

    LSF_ASSERT(Base64Decode(dec_buf, &dec_len, enc_buf, enc_len));
    cout << "dec len: " << dec_len << endl;
    LSF_ASSERT(dec_len == sizeof(content));
}

int main(int argc, char **argv)
{
	LSF_TEST_ALL(argc, argv);
}

// vim:ts=4:sw=4:et:ft=cpp:
