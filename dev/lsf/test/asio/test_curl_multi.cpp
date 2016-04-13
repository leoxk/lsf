// File:        test_curl_multi.cpp
// Description: ---
// Notes:       ---
// Author:      leoxiang <leoxiang727@qq.com>
// Revision:    2016-03-31 by leoxiang

#include "lsf/basic/unit_test.hpp"
#include "lsf/asio/curl_multi.hpp"

using namespace std;
using namespace lsf::asio;

#define URL "http://www.google.com"
#define VALID_URL "http://www.google.com/"
#define WRONG_NAME "http://127.0.0.1:1234"
#define WRONG_URL  "http://www.google.com/error"
#define DEF_TIMEOUT 5

LSF_TEST_CASE(test_multi_curl) {
    SharedCurl shared_curl;
    LSF_ASSERT(shared_curl.use_count() == 1);
    LSF_ASSERT(shared_curl->SetUrl(URL));
    LSF_ASSERT(shared_curl->SetTimeout(DEF_TIMEOUT));
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
    LSF_ASSERT(CurlMulti::Instance()->TimerSet().size() == 1);
    LSF_ASSERT(CurlMulti::Instance()->CurlMap().empty());
    LSF_ASSERT(IOService::Instance()->Driver()->GetRegisterEventSize() == 1);
    LSF_ASSERT(IOService::Instance()->Queue().GetReadCompletionMap().size() == 1);
    LSF_ASSERT(IOService::Instance()->Queue().GetWriteCompletionMap().size() == 0);
    LSF_ASSERT(IOService::Instance()->Queue().GetRawCompletionMap().size() == 0);
}

#define COUNT1 10
LSF_TEST_CASE(test_more_sophicate) {
    SharedCurl curls[COUNT1];
    for (int i = 0; i < COUNT1; ++i) {
        LSF_ASSERT(curls[i]->SetUrl(URL));
        LSF_ASSERT(curls[i]->SetTimeout(DEF_TIMEOUT));
        LSF_ASSERT(CurlMulti::Instance()->AddCurl(curls[i], [](SharedCurl curl) {
            LSF_ASSERT(curl->EffectiveUrl() == VALID_URL);
            LSF_ASSERT(curl.use_count() == 3);
            LSF_ASSERT(curl->Success());

            static int finish_count = 0;
            if (++finish_count == COUNT1) IOService::Instance()->SetExit();
        }));
    }
    IOService::Instance()->Run();

    // check status
    LSF_ASSERT(CurlMulti::Instance()->TimerSet().size() == 1);
    LSF_ASSERT(CurlMulti::Instance()->CurlMap().empty());
    LSF_ASSERT(IOService::Instance()->Driver()->GetRegisterEventSize() == 1);
    LSF_ASSERT(IOService::Instance()->Queue().GetReadCompletionMap().size() == 1);
    LSF_ASSERT(IOService::Instance()->Queue().GetWriteCompletionMap().size() == 0);
    LSF_ASSERT(IOService::Instance()->Queue().GetRawCompletionMap().size() == 0);
}

#define COUNT2 20
void AddCurl() {
    SharedCurl curl;
    LSF_ASSERT(curl->SetVerbose());
    LSF_ASSERT(curl->SetUrl(URL));
    LSF_ASSERT(curl->SetTimeout(DEF_TIMEOUT));
    LSF_ASSERT(CurlMulti::Instance()->AddCurl(curl, [](SharedCurl curl) {
            LSF_ASSERT(curl->EffectiveUrl() == VALID_URL);
            LSF_ASSERT(curl.use_count() == 2);
            LSF_ASSERT(curl->Success());
            static int count = 0;
            if (++count < COUNT2) {
                AddCurl();
            }
            else {
                IOService::Instance()->SetExit();
            }
        }));
}

// wil not pass
LSF_TEST_CASE(test_more_sophicate_sequencial) {
    AddCurl();
    IOService::Instance()->Run();

    // check status
    LSF_ASSERT(CurlMulti::Instance()->TimerSet().size() == 1);
    LSF_ASSERT(CurlMulti::Instance()->CurlMap().empty());
    LSF_ASSERT(IOService::Instance()->Driver()->GetRegisterEventSize() == 1);
    LSF_ASSERT(IOService::Instance()->Queue().GetReadCompletionMap().size() == 1);
    LSF_ASSERT(IOService::Instance()->Queue().GetWriteCompletionMap().size() == 0);
    LSF_ASSERT(IOService::Instance()->Queue().GetRawCompletionMap().size() == 0);

    std::cout << CurlMulti::Instance()->TimerSet().size() << std::endl
              << IOService::Instance()->Driver()->GetRegisterEventSize() << std::endl
              << IOService::Instance()->Queue().GetReadCompletionMap().size() << std::endl;
}

LSF_TEST_CASE(test_timeout) {
    // wrong name resolve
    SharedCurl shared_curl;
    LSF_ASSERT(shared_curl->SetUrl(WRONG_NAME));
    LSF_ASSERT(shared_curl->SetTimeout(DEF_TIMEOUT));
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
    LSF_ASSERT(shared_curl->SetTimeout(DEF_TIMEOUT));
    LSF_ASSERT(CurlMulti::Instance()->AddCurl(shared_curl, [&](SharedCurl curl) {
        LSF_ASSERT(curl == shared_curl);
        LSF_ASSERT(!curl->Output().empty());
        LSF_ASSERT(curl->Success());
        IOService::Instance()->SetExit();
    }));
    IOService::Instance()->Run();

    // check status
    LSF_ASSERT(CurlMulti::Instance()->TimerSet().size() == 1);
    LSF_ASSERT(CurlMulti::Instance()->CurlMap().empty());
    LSF_ASSERT(IOService::Instance()->Driver()->GetRegisterEventSize() == 1);
    LSF_ASSERT(IOService::Instance()->Queue().GetReadCompletionMap().size() == 1);
    LSF_ASSERT(IOService::Instance()->Queue().GetWriteCompletionMap().size() == 0);
    LSF_ASSERT(IOService::Instance()->Queue().GetRawCompletionMap().size() == 0);
}

int main(int argc, char **argv) {
    LSF_ASSERT(CurlMulti::Instance()->Init());
    LSF_TEST_ALL(argc, argv);
}

// vim:ts=4:sw=4:et:ft=cpp:
