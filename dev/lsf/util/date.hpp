// File:        date.hpp
// Description: ---
// Notes:       ---
// Author:      leoxiang <leoxiang727@qq.com>
// Revision:    2012-06-04 by leoxiang

#pragma once

#include <ctime>
#include <cstring>
#include "lsf/basic/error.hpp"
#include "lsf/basic/type_cast.hpp"
#include "lsf/util/string_ext.hpp"

namespace lsf {
namespace util {

class Date : public lsf::basic::Error {
public:
    const static time_t SECONDS_PER_DAY = 3600 * 24;
    const static time_t SECONDS_PER_MONTH = 30 * SECONDS_PER_DAY;

public:
    static Date Now() { return Date(); }
    static size_t DateTimeToInt(std::string const& date_time) {
        size_t pos = 0;
        size_t hour = lsf::basic::TypeCast<size_t>(lsf::util::StringExt::SplitGetNext(date_time, ':', pos, true));
        size_t minute = lsf::basic::TypeCast<size_t>(lsf::util::StringExt::SplitGetNext(date_time, ':', pos, true));
        size_t second = lsf::basic::TypeCast<size_t>(lsf::util::StringExt::SplitGetNext(date_time, ':', pos, true));
        return hour*3600 + minute*60 + second;
    }
    static bool IsSameDay(time_t time1, time_t time2) {
        Date d1(time1);
        Date d2(time2);
        return d1.GetYear() == d2.GetYear() &&
               d1.GetMonth() == d2.GetMonth() &&
               d1.GetDay() == d2.GetDay();
    }

public:
    Date(time_t calendar_time = ::time(nullptr)) { ::localtime_r(&calendar_time, &_date); }

    Date(tm const& date) : _date(date) {}

    // must be this format "year-month-day hour:minute:second"
    Date(std::string const& date_string) {
        std::string tmp = date_string;
        util::StringExt::RemoveHeadWhitespace(tmp);
        util::StringExt::RemoveTailWhitespace(tmp);
        size_t pos = 0;
        size_t year   = basic::TypeCast<size_t>(util::StringExt::SplitGetNext(tmp, "- :", pos, true));
        size_t month  = basic::TypeCast<size_t>(util::StringExt::SplitGetNext(tmp, "- :", pos, true));
        size_t day    = basic::TypeCast<size_t>(util::StringExt::SplitGetNext(tmp, "- :", pos, true));
        size_t hour   = basic::TypeCast<size_t>(util::StringExt::SplitGetNext(tmp, "- :", pos, true));
        size_t minute = basic::TypeCast<size_t>(util::StringExt::SplitGetNext(tmp, "- :", pos, true));
        size_t second = basic::TypeCast<size_t>(util::StringExt::SplitGetNext(tmp, "- :", pos, true));
        new (this) Date(year, month, day, hour, minute, second);
    }

    Date(size_t year, size_t month, size_t day, size_t hour, size_t minute, size_t second) {
        Date();
        _date.tm_year = year - 1900;
        _date.tm_mon = month - 1;
        _date.tm_mday = day;
        _date.tm_hour = hour;
        _date.tm_min = minute;
        _date.tm_sec = second;
    }

    // member funcs
    void SetYear(size_t year) { _date.tm_year = year - 1900; }
    void SetMonth(size_t month) { _date.tm_mon = month - 1; }
    void SetDay(size_t day) { _date.tm_mday = day; }
    void SetHour(size_t hour) { _date.tm_hour = hour; }
    void SetMinute(size_t minute) { _date.tm_min = minute; }
    void SetSecond(size_t second) { _date.tm_sec = second; }

    size_t GetYear() const { return _date.tm_year + 1900; }
    size_t GetMonth() const { return _date.tm_mon + 1; }
    size_t GetDay() const { return _date.tm_mday; }
    size_t GetHour() const { return _date.tm_hour; }
    size_t GetMinute() const { return _date.tm_min; }
    size_t GetSecond() const { return _date.tm_sec; }
    size_t GetWeekDay() const { return _date.tm_wday; }
    size_t GetYearDay() const { return _date.tm_yday; }

    std::string ToFormatString(char const *fmt) const {
        char tmp[512];
        ::strftime(tmp, sizeof(tmp), fmt, &_date);
        return std::string(tmp);
    }

    std::string ToString() const { return ToFormatString("%Y-%m-%d %H:%M:%S"); }

    time_t ToCalendarTime() { return ::mktime(&_date); }

    size_t DateTimeToInt() { return GetHour()*3600 + GetMinute()*60 + GetSecond(); }

private:
    struct tm _date;
};

}  // end of namespace util
}  // end of namespace lsf

// vim:ts=4:sw=4:et:ft=cpp:
