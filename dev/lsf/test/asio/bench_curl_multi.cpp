// File:        bench_curl_multi.cpp
// Description: ---
// Notes:       ---
// Author:      leoxiang <leoxiang727@qq.com>
// Revision:    2016-04-12 by leoxiang

#include "lsf/basic/unit_test.hpp"
#include "lsf/asio/curl_multi.hpp"

using namespace std;
using namespace lsf::asio;

#define URL "http://www.google.com"
#define DEF_TIMEOUT 5

void AddCurl() {
    SharedCurl curl;
    curl->SetUrl(URL);
    CurlMulti::Instance()->AddCurl(curl, [](SharedCurl curl) {
            curl->Success();
            static int count = 0;
            if (++count >= 10000) {
                IOService::Instance()->SetExit();
            }
            LSF_LOG_INF("curl success %d", count);
        });
    static int count = 0;
    LSF_LOG_INF("add curl %d", ++count);
}

LSF_TEST_CASE(test_more_sophicate_sequencial) {
    IOService::Instance()->AsyncAddTimerForever(50, [](int fd) {
            AddCurl();
        });
    IOService::Instance()->Run();
}

int main(int argc, char **argv) {
    CurlMulti::Instance()->Init();
    LSF_TEST_ALL(argc, argv);
}


// vim:ts=4:sw=4:et:ft=cpp:
