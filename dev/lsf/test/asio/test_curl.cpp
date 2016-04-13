// File:        test_curl.cpp
// Description: ---
// Notes:       ---
// Author:      leoxiang <leoxiang727@qq.com>
// Revision:    2015-08-25 by leoxiang

#include "lsf/basic/unit_test.hpp"
#include "lsf/asio/curl.hpp"

using namespace std;
using namespace lsf::asio;

#define URL "http://www.google.com"
#define VALID_URL "http://www.google.com/"

LSF_TEST_CASE(test_easy_curl) {
    Curl curl;
    LSF_ASSERT(curl);
    LSF_ASSERT(curl.SetUrl(URL));
    LSF_ASSERT(curl.EffectiveUrl() == URL);
    LSF_ASSERT(curl.SetOutputToString());
    LSF_ASSERT(curl.SetVerbose());
    LSF_ASSERT(curl.Perform());
    LSF_ASSERT(curl.EffectiveUrl() == VALID_URL);
    LSF_ASSERT(curl.Success());

    // check status
    LSF_ASSERT(IOService::Instance()->Driver()->GetRegisterEventSize() == 0);
    LSF_ASSERT(IOService::Instance()->Queue().GetReadCompletionMap().size() == 0);
    LSF_ASSERT(IOService::Instance()->Queue().GetWriteCompletionMap().size() == 0);
    LSF_ASSERT(IOService::Instance()->Queue().GetRawCompletionMap().size() == 0);
}

int main(int argc, char **argv) { LSF_TEST_ALL(argc, argv); }

// vim:ts=4:sw=4:et:ft=cpp:
