// File:        test_log.cpp
// Description: ---
// Notes:       ---
// Author:      leoxiang <leoxiang727@qq.com>
// Revision:    2012-05-14 by leoxiang

#include "lsf/basic/unit_test.hpp"
#include "lsf/util/log.hpp"

using namespace std;
using namespace lsf::util;

LSF_TEST_CASE(bind_to_file) {
    Log log;

    // open file and output
    LSF_ASSERT(log.BindOutput(new FileLogDriver("/dev/null", FileLogDriver::SHIFT_NONE)));
    LSF_ASSERT(log.IsBindOuput());
    LSF_ASSERT(log.WriteLog(Log::TYPE_WARN, "some test info"));

    // open a non-exist file
    LSF_ASSERT(!log.BindOutput(new FileLogDriver("/none/exist/file")));
    LSF_ASSERT(!log.IsBindOuput());
}

LSF_TEST_CASE(bind_to_terminal) {
    Log log;

    LSF_ASSERT(log.BindOutput(new TermLogDriver()));
    LSF_ASSERT(log.IsBindOuput());
    LSF_ASSERT(log.WriteLog(Log::TYPE_INFO, "test log to terminal out"));
    LSF_ASSERT(log.WriteLog(Log::TYPE_INFO, "test log to terminal err"));
}

LSF_TEST_CASE(test_macro) {
    LSF_ASSERT(!SingleLog::Instance()->IsBindOuput());

    // file
    LSF_ASSERT(SingleLog::Instance()->BindOutput(new FileLogDriver("/dev/null", FileLogDriver::SHIFT_NONE)));
    LSF_ASSERT(SingleLog::Instance()->IsBindOuput());
    LSF_ASSERT(LSF_LOG_INFO("test SingleLog info"));
    LSF_ASSERT(LSF_LOG_DEBUG("test SingleLog debug"));
    LSF_ASSERT(LSF_LOG_WARN("test SingleLog warn"));
    LSF_ASSERT(LSF_LOG_ERR("test SingleLog err"));
    LSF_ASSERT(LSF_LOG_FATAL("test SingleLog fatal"));

    // term
    LSF_ASSERT(SingleLog::Instance()->BindOutput(new TermLogDriver()));
    LSF_ASSERT(SingleLog::Instance()->IsBindOuput());
    LSF_ASSERT(LSF_LOG_INFO("test terminal SingleLog info"));
    LSF_ASSERT(LSF_LOG_DEBUG("test terminal SingleLog debug"));
    LSF_ASSERT(LSF_LOG_WARN("test terminal SingleLog warn"));
    LSF_ASSERT(LSF_LOG_ERR("test terminal SingleLog err"));
    LSF_ASSERT(LSF_LOG_FATAL("test terminal SingleLog fatal"));
    LSF_ASSERT(LSF_LOG_FATAL("%s %u %d", "some thing", 100, 100));
}

int main(int argc, char **argv) { LSF_TEST_ALL(argc, argv); }

// vim:ts=4:sw=4:et:ft=cpp:
