// File:        test_log.cpp
// Description: ---
// Notes:       ---
// Author:      leoxiang <leoxiang727@qq.com>
// Revision:    2015-09-08 by leoxiang

#include "lsf/basic/unit_test.hpp"
#include "lsf/util/system.hpp"
#include "lsf/util/log.hpp"

using namespace std;
using namespace lsf::util;
char* exe_path = nullptr;

LSF_TEST_CASE(bind_to_terminal) {
    // output
    LSF_LOG_INF("test log to terminal inf");
    LSF_LOG_DBG("test log to terminal dbg");
    LSF_LOG_WRN("test log to terminal wrn");
    LSF_LOG_ERR("test log to terminal err");
    LSF_LOG_FAT("test log to terminal fat");

    // output with stack
    LSF_LOG_INF_STACK("test log to terminal inf");
    LSF_LOG_DBG_STACK("test log to terminal dbg");
    LSF_LOG_WRN_STACK("test log to terminal wrn");
    LSF_LOG_ERR_STACK("test log to terminal err");
    LSF_LOG_FAT_STACK("test log to terminal fat");
}

LSF_TEST_CASE(bind_to_string) {
    Log log;
    log.BindString();
    log.Output(Log::TYPE_INF, "output to string\n");
    LSF_ASSERT(log.GetString() == "output to string\n");

    log.BindString();
    log.Output(Log::TYPE_INF, "output to string");
    LSF_ASSERT(log.GetString() == "output to string");
}

LSF_TEST_CASE(bind_to_file) {
    Log log;
    // open a non-exist file
    LSF_ASSERT(!log.BindFile("/none/exist/file"));

    std::string file_raw = "./test_bind_file_log";
    std::string file = file_raw + Date().ToFormatString(".%Y-%m-%d");

    LSF_ASSERT(log.BindFile(file_raw, LogFileBuf::SHIFT_DAY));
    log << "test\ntest" << std::endl;
    LSF_ASSERT(System::IsFile(file));
    LSF_ASSERT(System::Rm(file));
}

// LSF_TEST_CASE(shift) {
//     // bind file log
//     LSF_ASSERT(LSF_LOG.BindFile("./tmp", lsf::util::LogFileBuf::SHIFT_DAY));
//
//     // repeated log
//     while (true) {
//         sleep(5);
//         LSF_LOG_INF("test log to terminal inf");
//     }
// }

int main(int argc, char **argv) {
    exe_path = argv[0];
    LSF_TEST_ALL(argc, argv);
}

// vim:ts=4:sw=4:et:ft=cpp:
