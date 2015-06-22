// File:        unit_test.hpp
// Description: ---
// Notes:       ---
// Author:      leoxiang <leoxiang727@qq.com>
// Revision:    2012-01-19 by leoxiang

#pragma once

#include <cstddef>
#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <sys/time.h>
#include "lsf/basic/singleton.hpp"

////////////////////////////////////////////////////////////
// Macro defines
#define LSF_TEST_RED "\e[1m\e[31m"
#define LSF_TEST_GREEN "\e[1m\e[32m"
#define LSF_TEST_EOC "\e[m"

// run test
#define LSF_TEST_ALL(argc, argv) \
    if (!::lsf::basic::UnitTest::Instance()->Run(argc, argv)) exit(-1);

// declare test case
#define LSF_TEST_CASE(arg_case_name)                                                                    \
    class LSF_TEST_##arg_case_name : public ::lsf::basic::TestCase {                                    \
    public:                                                                                             \
        explicit LSF_TEST_##arg_case_name(std::string const& case_name) : TestCase(case_name) {}        \
        virtual void Run();                                                                             \
    private:                                                                                            \
        static ::lsf::basic::TestCase* const _helper;                                                   \
    };                                                                                                  \
    ::lsf::basic::TestCase* const LSF_TEST_##arg_case_name::_helper =                                   \
        ::lsf::basic::UnitTest::Instance()->RegisterCase(new LSF_TEST_##arg_case_name(#arg_case_name)); \
    void LSF_TEST_##arg_case_name::Run()

// declare assert
#define LSF_ASSERT(expr)                                                                                         \
    do {                                                                                                         \
        if (expr)                                                                                                \
            std::cout << LSF_TEST_GREEN << "[  Passed  ] " << LSF_TEST_EOC << __FILE__ << "|" << __LINE__ << " " \
                      << #expr << std::endl;                                                                     \
        else {                                                                                                   \
            std::cout << LSF_TEST_RED << "[  Failed  ] " << LSF_TEST_EOC << __FILE__ << "|" << __LINE__ << " "   \
                      << #expr << std::endl;                                                                     \
            lsf::basic::lsf_test_case_result = false;                                                            \
        }                                                                                                        \
    } while (0)

#define LSF_ASSERT_EXIT(expr)                                                                                    \
    do {                                                                                                         \
        if (expr)                                                                                                \
            std::cout << LSF_TEST_GREEN << "[  Passed  ] " << LSF_TEST_EOC << __FILE__ << "|" << __LINE__ << " " \
                      << #expr << std::endl;                                                                     \
        else {                                                                                                   \
            std::cout << LSF_TEST_RED << "[  Failed  ] " << LSF_TEST_EOC << __FILE__ << "|" << __LINE__ << " "   \
                      << #expr << std::endl;                                                                     \
            lsf::basic::lsf_test_case_result = false;                                                            \
            exit(-1);                                                                                            \
        }                                                                                                        \
    } while (0)

#define LSF_ASSERT_ERR_ONLY(expr)                                                                              \
    do {                                                                                                       \
        if (!(expr)) {                                                                                         \
            std::cout << LSF_TEST_RED << "[  Failed  ] " << LSF_TEST_EOC << __FILE__ << "|" << __LINE__ << " " \
                      << #expr << std::endl;                                                                   \
            lsf::basic::lsf_test_case_result = false;                                                          \
        }                                                                                                      \
    } while (0)

#define LSF_ASSERT_EXIT_ERR_ONLY(expr)                                                                         \
    do {                                                                                                       \
        if (!(expr)) {                                                                                         \
            std::cout << LSF_TEST_RED << "[  Failed  ] " << LSF_TEST_EOC << __FILE__ << "|" << __LINE__ << " " \
                      << #expr << std::endl;                                                                   \
            lsf::basic::lsf_test_case_result = false;                                                          \
            exit(-1);                                                                                          \
        }                                                                                                      \
    } while (0)

////////////////////////////////////////////////////////////
// Class declare
namespace lsf {
namespace basic {

namespace {
static bool lsf_test_case_result = true;
}  // end of namespace anonymous

class TestCase {
public:
    TestCase(std::string const& case_name) : _case_name(case_name) {}

    virtual ~TestCase() {}

    virtual void Run() = 0;

    std::string const& CaseName() const { return _case_name; }

protected:
    std::string _case_name;
};

class UnitTest : public basic::Singleton<UnitTest> {
public:
    typedef std::vector<TestCase*> container_type;
    typedef container_type::const_iterator const_iterator;
    typedef container_type::iterator iterator;

public:
    TestCase* RegisterCase(TestCase* ptr_test_case) {
        _case_list.push_back(ptr_test_case);
        return ptr_test_case;
    }

    bool Run(int argc, char** argv) {
        size_t passed_num = 0;
        size_t failed_num = 0;
        size_t cnt = 1;
        bool result = true;
        timeval tv_begin, tv_end;

        // top delimit
        std::cout << LSF_TEST_GREEN << "============================================================" << LSF_TEST_EOC
                  << std::endl;
        std::cout << LSF_TEST_GREEN << "Start Testing " << argv[0] << LSF_TEST_EOC << std::endl;
        std::cout << LSF_TEST_GREEN << "============================================================" << LSF_TEST_EOC
                  << std::endl;

        // unit test content
        gettimeofday(&tv_begin, nullptr);
        for (UnitTest::iterator it = _case_list.begin(); it != _case_list.end(); it++, cnt++) {
            lsf::basic::lsf_test_case_result = true;

            timeval case_begin, case_end;

            std::cout << LSF_TEST_GREEN << "[ Test " << std::setw(3) << std::setfill('0') << cnt << " ] "
                      << (*it)->CaseName() << LSF_TEST_EOC << std::endl;

            gettimeofday(&case_begin, nullptr);

            (*it)->Run();

            gettimeofday(&case_end, nullptr);

            uint64_t milli_sec = (case_end.tv_sec * 1000 + case_end.tv_usec / 1000) -
                                 (case_begin.tv_sec * 1000 + case_begin.tv_usec / 1000);

            if (lsf::basic::lsf_test_case_result) {
                std::cout << LSF_TEST_GREEN << "[  Result  ] Passed" << LSF_TEST_EOC << std::endl;
                std::cout << LSF_TEST_GREEN << "[   Time   ] " << milli_sec / 1000 << "." << milli_sec % 1000
                          << LSF_TEST_EOC << std::endl;
                passed_num++;
            } else {
                std::cout << LSF_TEST_RED << "[  Result  ] Failed" << LSF_TEST_EOC << std::endl;
                std::cout << LSF_TEST_RED << "[   Time   ] " << milli_sec / 1000 << "." << milli_sec % 1000
                          << LSF_TEST_EOC << std::endl;
                failed_num++;
                result = false;
            }
        }
        gettimeofday(&tv_end, nullptr);

        // middle delimit
        uint64_t milli_sec =
            (tv_end.tv_sec * 1000 + tv_end.tv_usec / 1000) - (tv_begin.tv_sec * 1000 + tv_begin.tv_usec / 1000);

        // unit test statics
        std::cout << LSF_TEST_GREEN << "============================================================" << LSF_TEST_EOC
                  << std::endl;
        std::cout << LSF_TEST_GREEN << "Total TestCase: " << passed_num + failed_num << LSF_TEST_EOC << std::endl;
        std::cout << LSF_TEST_GREEN << "Total Time: " << milli_sec / 1000 << "." << milli_sec % 1000 << LSF_TEST_EOC
                  << std::endl;
        std::cout << LSF_TEST_GREEN << "Total Passed: " << passed_num << LSF_TEST_EOC << std::endl;
        std::cout << LSF_TEST_RED << "Total Failed: " << failed_num << LSF_TEST_EOC << std::endl;

        return result;
    }

private:
    container_type _case_list;
};

}  // end of namespace basic
}  // end of namespace lsf

// vim:ts=4:sw=4:et:ft=cpp:
