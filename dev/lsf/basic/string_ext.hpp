// File:        string_ext.hpp
// Description: ---
// Notes:       ---
// Author:      leoxiang <leoxiang727@qq.com>
// Revision:    2012-05-19 by leoxiang

#pragma once

#include <iostream>
#include <stdarg.h>
#include <cstdio>
#include <string>
#include <cstring>
#include <sstream>

namespace lsf {
namespace basic {

namespace detail {

    static const std::string WHITE_SPACES = " \r\n\f\t\v";

} // end of namespace detail

class StringExt
{
public:
    ////////////////////////////////////////////////////////////
    // split
    static std::string SplitAndGet(std::string const & input, std::string const & delimit, size_t offset)
    {
        size_t off_mark = 0;
        size_t last = 0;
        size_t pos = input.find_first_of(delimit);
        while (last != std::string::npos)
        {
            if (off_mark++ == offset)
                return input.substr(last, pos - last);
            last = input.find_first_not_of(delimit, pos);
            pos  = input.find_first_of(delimit, last);
        }
        return "";
    }

    static std::string SplitAndGet(std::string const & input, char delimit, size_t offset)
    {
        return SplitAndGet(input, std::string(1, delimit), offset);
    }
    ////////////////////////////////////////////////////////////
    // replace
    static std::string & ReplaceInplace(std::string & src, 
            std::string const & to_replace, std::string const & replace_with)
    {
        for (size_t pos = src.find(to_replace); 
                pos != std::string::npos; 
                pos = src.find(to_replace, pos))
        {
            src.replace(pos, to_replace.size(), replace_with);
            pos += replace_with.size();
        }
        return src;
    }

    static std::string Replace(std::string const & src, 
            std::string const & to_replace, std::string const & replace_with)
    {
        std::string tmp;
        size_t last = 0;
        size_t pos = src.find(to_replace);
        while (pos != std::string::npos)
        {
            tmp += src.substr(last, pos - last);
            tmp += replace_with;
            last = pos + to_replace.size();
            pos  = src.find(to_replace, last);
        }
        tmp += src.substr(last);;
        return tmp;
    }
    ////////////////////////////////////////////////////////////
    // strip
    static std::string & RemoveHeadWhitespace(std::string & input)
    {
        size_t pos = input.find_first_not_of(detail::WHITE_SPACES);

        if (pos == 0)
        {
            return input;
        }
        else if (pos == std::string::npos)
        {
            input.clear();
        }
        else 
        {
            input.erase(0, pos);
        }
        return input;
    }

    static std::string & RemoveTailWhitespace(std::string & input)
    {
        size_t pos = input.find_last_not_of(detail::WHITE_SPACES);

        if (pos == input.size() - 1)
        {
            return input;
        }
        else if (pos == std::string::npos)
        {
            input.clear();
        }
        else 
        {
            input.erase(pos + 1);
        }
        return input;
    }

    ////////////////////////////////////////////////////////////
    // path related
    static std::string GetDirName(std::string const & input)
    {
        // remove heading and tailing whitespace
        size_t pos1 = input.find_first_not_of(detail::WHITE_SPACES);
        size_t pos2 = input.find_last_not_of(detail::WHITE_SPACES);
        // remove tailing '/'
        while (input[pos2] == '/' && pos2 != 0)
        {
            pos2--;
        }

        // find last '/'
        size_t pos = input.find_last_of('/', pos2);

        if (pos == std::string::npos)
        {
            return std::string(".");
        }
        else if (pos == pos1)
        {
            return std::string("/");
        }
        else
        {
            return input.substr(pos1, pos - pos1);
        }
        return input;
    }

    static std::string GetBaseName(std::string const & input)
    {
        // remove heading and tailing whitespace
        size_t pos1 = input.find_first_not_of(detail::WHITE_SPACES);
        size_t pos2 = input.find_last_not_of(detail::WHITE_SPACES);
        // remove tailing '/'
        while (input[pos2] == '/' && pos2 != 0) pos2--;

        // find last '/'
        size_t pos = input.find_last_of('/', pos2);

        if (pos == std::string::npos)
        {
            return input.substr(pos1, pos2 - pos1 + 1);
        }
        else
        {
            return input.substr(pos + 1, pos2 - pos);
        }
        return input;
    }

    static std::string GetSuffix(std::string const & input)
    {
        std::string tmp = GetBaseName(input);

        size_t pos = tmp.find_last_of('.');

        if (pos == std::string::npos)
        {
            return std::string("");
        }
        else
        {
            return tmp.substr(pos + 1);
        }
        return tmp;
    }

    ////////////////////////////////////////////////////////////
    // other
    static std::string Format(char const * fmt, ...)
    {
        char tmp[8192];

        va_list ap;
        va_start(ap, fmt);
        std::vsprintf(tmp, fmt, ap);
        va_end(ap);

        return std::string(tmp);
    }
};

} // end of namespace basic
} // end of namespace lsf


// vim:ts=4:sw=4:et:ft=cpp:
