// File:        test_buffer.cpp
// Description: ---
// Notes:       ---
// Author:      leoxiang <leoxiang727@qq.com>
// Revision:    2012-06-02 by leoxiang

#include "lsf/basic/unit_test.hpp"
#include "lsf/basic/buffer.hpp"

using namespace std;
using namespace lsf::basic;

LSF_TEST_CASE(test_static_funcs)
{
    const unsigned char content[] = {
        0x16, 0x34, 0xa8, 0xc0, 0x1f, 0x12, 0x12, 0x94,
        0x16, 0xb8, 0xa8, 0x12, 0x12, 0xcc, 0xc1, 0xba,
        0x16, 0x82, 0x12, 0xb7, 0x94, 0xaa, 0x94, 0xa1,
        0x16, 0xaa, 0xb7, 0xb7, 0x12, 0xaa, 0xba, 0x94
    };

    LSF_ASSERT(BinToHexString(content, sizeof(content)) == "1634a8c01f12129416b8a81212ccc1ba168212b794aa94a116aab7b712aaba94");
    LSF_ASSERT(BinToString(content, sizeof(content)) == ".4..............................");

    Buffer<1024> buf;
    LSF_ASSERT(HexStringToBin(BinToHexString(content, sizeof(content)), buf));
    LSF_ASSERT(buf.Size() == 32);
    LSF_ASSERT(Buffer<1024>(content, sizeof(content)).ToHexString() == buf.ToHexString());
    LSF_ASSERT(Buffer<1024>(content, sizeof(content)) == buf);
}

LSF_TEST_CASE(test_with_static_bytes)
{
    Buffer<16>   bytes;

    // binary bytes
    bytes.Fill(0xff);
    LSF_ASSERT(bytes.ToHexString() == "ffffffffffffffffffffffffffffffff");
    LSF_ASSERT(bytes.ToString() == "................");

    // length >= 16
    bytes.Copy("this is a string");
    LSF_ASSERT(bytes.ToHexString() == "74686973206973206120737472696e67");
    LSF_ASSERT(bytes.ToString() == "this is a string");

    // length < 16
    bytes.Copy("short string");
    LSF_ASSERT(string((char *)bytes.Data(), bytes.Size()) == "short string");
    LSF_ASSERT(bytes.ToHexString() == "73686f727420737472696e67");
}

int main(int argc, char **argv)
{
	LSF_TEST_ALL();
}

// vim:ts=4:sw=4:et:ft=cpp:
