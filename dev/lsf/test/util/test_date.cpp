// File:        test_date.cpp
// Description: ---
// Notes:       ---
// Author:      leoxiang <leoxiang727@qq.com>
// Revision:    2012-06-04 by leoxiang

#include "lsf/basic/unit_test.hpp"
#include "lsf/util/date.hpp"

using namespace std;
using namespace lsf::util;

LSF_TEST_CASE(test) {
    Date date(2011, 11, 11, 11, 11, 11);

    LSF_ASSERT(date.GetYear() == 2011);
    LSF_ASSERT(date.GetMonth() == 11);
    LSF_ASSERT(date.GetDay() == 11);
    LSF_ASSERT(date.GetHour() == 11);
    LSF_ASSERT(date.GetMinute() == 11);
    LSF_ASSERT(date.GetSecond() == 11);

    time_t calendar_time = time(nullptr);
    LSF_ASSERT(Date(calendar_time).ToCalendarTime() == calendar_time);

    LSF_ASSERT(date.ToFormatString("%Y-%m-%d %H:%M:%S") == "2011-11-11 11:11:11");
    LSF_ASSERT(date.ToFormatString("%Y-%m-%d %H:%M:%S") == date.ToString());
}

LSF_TEST_CASE(test_create_from_string) {
    Date date("2011-11-11 11:11:11");

    LSF_ASSERT(date.ToString() == "2011-11-11 11:11:11");
    LSF_ASSERT(date.GetYear() == 2011);
    LSF_ASSERT(date.GetMonth() == 11);
    LSF_ASSERT(date.GetDay() == 11);
    LSF_ASSERT(date.GetHour() == 11);
    LSF_ASSERT(date.GetMinute() == 11);
    LSF_ASSERT(date.GetSecond() == 11);

    LSF_ASSERT(Date("  2011-11-11   11:11:11  ").ToString() == "2011-11-11 11:11:11");

    LSF_ASSERT(Date(Date("2016-08-08 08:08:08").ToCalendarTime()).ToString() == "2016-08-08 08:08:08");
    LSF_ASSERT(Date(Date("2016-09-09 09:09:09").ToCalendarTime()).ToString() == "2016-09-09 09:09:09");
}

int main(int argc, char **argv) { LSF_TEST_ALL(argc, argv); }

// vim:ts=4:sw=4:et:ft=cpp:
