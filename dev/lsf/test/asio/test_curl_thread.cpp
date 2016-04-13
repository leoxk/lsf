// File:        test_curl_thread.cpp
// Description: ---
// Notes:       ---
// Author:      leoxiang <leoxiang727@qq.com>
// Revision:    2016-03-31 by leoxiang

#include "lsf/basic/unit_test.hpp"
#include "lsf/asio/curl_thread.hpp"

using namespace std;
using namespace lsf::asio;

#define URL "http://www.google.com"
#define VALID_URL "http://www.google.com/"
#define WRONG_NAME "http://127.0.0.1:1234"
#define WRONG_URL  "http://www.google.com/error"
#define DEF_THREAD_NUM 10

LSF_TEST_CASE(test_multi_curl) {
    SharedCurl shared_curl;
    LSF_ASSERT(shared_curl->SetUrl(URL));
    LSF_ASSERT(CurlThread::Instance()->AddCurl(shared_curl, [&](SharedCurl curl) {
        LSF_ASSERT(curl->EffectiveUrl() == VALID_URL);
        LSF_ASSERT(curl == shared_curl);
        LSF_ASSERT(curl->Success());
        IOService::Instance()->SetExit();
    }));

    IOService::Instance()->Run();

    // check status
    LSF_ASSERT(CurlThread::Instance()->GetFuncMap().empty());
    LSF_ASSERT(CurlThread::Instance()->GetInputList().empty());
    LSF_ASSERT(CurlThread::Instance()->GetOutputList().empty());
}

LSF_TEST_CASE(test_timeout) {
    // wrong name resolve
    SharedCurl shared_curl;
    LSF_ASSERT(shared_curl->SetUrl(WRONG_NAME));
    LSF_ASSERT(CurlThread::Instance()->AddCurl(shared_curl, [&](SharedCurl curl) {
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
    LSF_ASSERT(CurlThread::Instance()->AddCurl(shared_curl, [&](SharedCurl curl) {
        LSF_ASSERT(curl == shared_curl);
        LSF_ASSERT(!curl->Output().empty());
        LSF_ASSERT(curl->Success());
        IOService::Instance()->SetExit();
    }));
    IOService::Instance()->Run();

    // check status
    LSF_ASSERT(CurlThread::Instance()->GetFuncMap().empty());
    LSF_ASSERT(CurlThread::Instance()->GetInputList().empty());
    LSF_ASSERT(CurlThread::Instance()->GetOutputList().empty());
}

#define COUNT 10
LSF_TEST_CASE(test_more_sophicate) {
    SharedCurl curls[COUNT];
    for (int i = 0; i < COUNT; ++i) {
        LSF_ASSERT(curls[i]->SetUrl(URL));
        LSF_ASSERT(CurlThread::Instance()->AddCurl(curls[i], [](SharedCurl curl) {
            LSF_ASSERT(curl->EffectiveUrl() == VALID_URL);
            LSF_ASSERT(curl->Success());

            static int finish_count = 0;
            if (++finish_count == COUNT) IOService::Instance()->SetExit();
        }));
    }
    IOService::Instance()->Run();

    // check status
    LSF_ASSERT(CurlThread::Instance()->GetFuncMap().empty());
    LSF_ASSERT(CurlThread::Instance()->GetInputList().empty());
    LSF_ASSERT(CurlThread::Instance()->GetOutputList().empty());
}

int main(int argc, char **argv) {
    CurlThread::Instance()->Init(DEF_THREAD_NUM);
    LSF_TEST_ALL(argc, argv);
}

// vim:ts=4:sw=4:et:ft=cpp:
