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

#define WRONG_NAME "http://127.0.0.1:1234"
#define WRONG_URL  "http://www.google.com/error"

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

LSF_TEST_CASE(test_multi_curl) {
    LSF_ASSERT(CurlMulti::Instance()->Init());

    SharedCurl shared_curl;
    LSF_ASSERT(shared_curl.use_count() == 1);
    LSF_ASSERT(shared_curl->SetUrl(URL));
    LSF_ASSERT(CurlMulti::Instance()->AddCurl(shared_curl, [&](SharedCurl curl) {
        LSF_ASSERT(curl->EffectiveUrl() == VALID_URL);
        LSF_ASSERT(curl.use_count() == 3);
        LSF_ASSERT(curl == shared_curl);
        LSF_ASSERT(curl->Success());
        IOService::Instance()->SetExit();
    }));

    IOService::Instance()->Run();
    LSF_ASSERT(shared_curl.use_count() == 1);

    // check status
    LSF_ASSERT(CurlMulti::Instance()->TimerSet().empty());
    LSF_ASSERT(CurlMulti::Instance()->CurlMap().empty());
    LSF_ASSERT(IOService::Instance()->Driver()->GetRegisterEventSize() == 0);
    LSF_ASSERT(IOService::Instance()->Queue().GetReadCompletionMap().size() == 0);
    LSF_ASSERT(IOService::Instance()->Queue().GetWriteCompletionMap().size() == 0);
    LSF_ASSERT(IOService::Instance()->Queue().GetRawCompletionMap().size() == 0);
}

#define COUNT 10
LSF_TEST_CASE(test_more_sophicate) {
    LSF_ASSERT(CurlMulti::Instance()->Init());

    SharedCurl curls[COUNT];
    for (int i = 0; i < COUNT; ++i) {
        LSF_ASSERT(curls[i]->SetUrl(URL));
        LSF_ASSERT(CurlMulti::Instance()->AddCurl(curls[i], [](SharedCurl curl) {
            LSF_ASSERT(curl->EffectiveUrl() == VALID_URL);
            LSF_ASSERT(curl.use_count() == 3);
            LSF_ASSERT(curl->Success());

            static int finish_count = 0;
            if (++finish_count == COUNT) IOService::Instance()->SetExit();
        }));
    }
    IOService::Instance()->Run();

    // check status
    LSF_ASSERT(CurlMulti::Instance()->TimerSet().empty());
    LSF_ASSERT(CurlMulti::Instance()->CurlMap().empty());
    LSF_ASSERT(IOService::Instance()->Driver()->GetRegisterEventSize() == 0);
    LSF_ASSERT(IOService::Instance()->Queue().GetReadCompletionMap().size() == 0);
    LSF_ASSERT(IOService::Instance()->Queue().GetWriteCompletionMap().size() == 0);
    LSF_ASSERT(IOService::Instance()->Queue().GetRawCompletionMap().size() == 0);
}

LSF_TEST_CASE(test_timeout) {
    LSF_ASSERT(CurlMulti::Instance()->Init());

    // wrong name resolve
    SharedCurl shared_curl;
    LSF_ASSERT(shared_curl->SetUrl(WRONG_NAME));
    LSF_ASSERT(CurlMulti::Instance()->AddCurl(shared_curl, [&](SharedCurl curl) {
        LSF_ASSERT(curl == shared_curl);
        LSF_ASSERT(curl->Output().empty());
        LSF_ASSERT(!curl->Success());
        std::cout << curl->EffectiveUrl() << std::endl;
        std::cout << curl->ErrString() << std::endl;
        IOService::Instance()->SetExit();
    }));
    IOService::Instance()->Run();

    // wrong url
    shared_curl->Reset();
    LSF_ASSERT(shared_curl->SetUrl(WRONG_URL));
    LSF_ASSERT(CurlMulti::Instance()->AddCurl(shared_curl, [&](SharedCurl curl) {
        LSF_ASSERT(curl == shared_curl);
        LSF_ASSERT(!curl->Output().empty());
        LSF_ASSERT(curl->Success());
        IOService::Instance()->SetExit();
    }));
    IOService::Instance()->Run();

    // check status
    LSF_ASSERT(CurlMulti::Instance()->TimerSet().empty());
    LSF_ASSERT(CurlMulti::Instance()->CurlMap().empty());
    LSF_ASSERT(IOService::Instance()->Driver()->GetRegisterEventSize() == 0);
    LSF_ASSERT(IOService::Instance()->Queue().GetReadCompletionMap().size() == 0);
    LSF_ASSERT(IOService::Instance()->Queue().GetWriteCompletionMap().size() == 0);
    LSF_ASSERT(IOService::Instance()->Queue().GetRawCompletionMap().size() == 0);
}

int main(int argc, char **argv) { LSF_TEST_ALL(argc, argv); }

// vim:ts=4:sw=4:et:ft=cpp:
