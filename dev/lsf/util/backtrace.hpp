// File:        backtrace.hpp
// Description: ---
// Notes:       ---
// Author:      leoxiang <leoxiang727@qq.com>
// Revision:    2015-06-15 by leoxiang

#pragma once

#include <execinfo.h>
#include <cxxabi.h>
#include <exception>
#include <string>
#include "lsf/basic/singleton.hpp"
#include "lsf/util/log.hpp"

namespace lsf {
namespace util {

class Backtrace : public basic::Singleton<Backtrace> {
public:
    const static int MAX_STACK_SIZE = 20;

public:
    std::string const& ToString(int count = MAX_STACK_SIZE) {
        static std::string content;
        content.clear();
        count++;

        // check input
        if (count > MAX_STACK_SIZE) count = MAX_STACK_SIZE;

        // get stack
        void* array[MAX_STACK_SIZE];
        int size = ::backtrace(array, count);
        char ** messages = ::backtrace_symbols(array, size);

        // compose string
        for (int i = 1; i < size; ++i) {
            char* message = messages[i];
            std::string tmp(message);

            // demangle
            size_t begin_name = tmp.find_first_of('(');
            size_t end_name = tmp.find_first_of('+');
            if (begin_name != std::string::npos && end_name != std::string::npos && begin_name < end_name) {
                int status;
                message[end_name++] = '\0';
                message[begin_name++] = '\0';
                char* realname = ::abi::__cxa_demangle(message + begin_name, nullptr, nullptr, &status);
                if (status == 0) {
                    tmp.clear();
                    tmp.append(message).append(1, '(').append(realname).append(1, '+').append(message + end_name);
                    free(realname);
                }
            }

            // append
            content.append(tmp);
            if (i != size - 1) content.append("\n");
        }

        return content;
    }

private:
};

////////////////////////////////////////////////////////////
// macro
#define LSF_BACKTRACE() lsf::util::Backtrace::Instance()->ToString(5)

#define LSF_LOG_INFO_WITH_STACK(fmt,args...)  LSF_LOG_INFO(fmt,##args);  LSF_LOG_INFO("%s",  LSF_BACKTRACE().c_str())

#define LSF_LOG_DEBUG_WITH_STACK(fmt,args...) LSF_LOG_DEBUG(fmt,##args); LSF_LOG_DEBUG("%s", LSF_BACKTRACE().c_str())

#define LSF_LOG_WARN_WITH_STACK(fmt,args...)  LSF_LOG_WARN(fmt,##args);  LSF_LOG_WARN("%s",  LSF_BACKTRACE().c_str())

#define LSF_LOG_ERR_WITH_STACK(fmt,args...)   LSF_LOG_ERR(fmt,##args);   LSF_LOG_ERR("%s",   LSF_BACKTRACE().c_str())

#define LSF_LOG_FATAL_WITH_STACK(fmt,args...) LSF_LOG_FATAL(fmt,##args); LSF_LOG_FATAL("%s", LSF_BACKTRACE().c_str())

}  // end of namespace util
}  // end of namespace lsf

// vim:ts=4:sw=4:et:ft=cpp:
