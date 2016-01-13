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
#include <vector>

namespace lsf {
namespace util {

////////////////////////////////////////////////////////////
// StringExt
////////////////////////////////////////////////////////////
class StringExt {
public:
    constexpr static char const* WHITE_SPACES = " \r\n\f\t\v";

public:
    ////////////////////////////////////////////////////////////
    // split get
    template<typename DelimitType>
    static std::string SplitGet(std::string const& input, DelimitType const& delimit, size_t index, bool ignore_repeated = false) {
        size_t count = 0;
        size_t begin = 0;
        size_t end = input.find_first_of(delimit);
        while (begin != std::string::npos) {
            if (end == std::string::npos) {
                if (count++ == index) return input.substr(begin);
                break;
            }
            else {
                if (count++ == index) return input.substr(begin, end - begin);
                begin = ignore_repeated ? input.find_first_not_of(delimit, end) : end + 1;
                end = input.find_first_of(delimit, begin);
            }
        }
        return "";
    }

    template<typename DelimitType>
    static std::string SplitGetNext(std::string const& input, DelimitType const& delimit, size_t& offset, bool ignore_repeated = false) {
        size_t begin = offset;
        size_t end = input.find_first_of(delimit, offset);
        if (begin != std::string::npos) {
            if (end == std::string::npos) {
                offset = std::string::npos;
                return input.substr(begin);
            }
            else {
                offset = ignore_repeated ? input.find_first_not_of(delimit, end) : end + 1;
                return input.substr(begin, end - begin);
            }
        }
        return "";
    }

	template<typename DelimitType>
	static std::vector<std::string> SplitStr(std::string const& input, DelimitType const& delimit, bool ignore_repeated = false){
		std::vector<std::string> out_vec;
		out_vec.clear();

		size_t begin = 0;
		size_t end = input.find_first_of(delimit);
		while(std::string::npos != begin){
			if(std::string::npos == end){
				out_vec.push_back(input.substr(begin));
				break;
			} else {
				out_vec.push_back(input.substr(begin, end-begin));
				begin = ignore_repeated ? input.find_first_not_of(delimit, end) : (end+1);
				end = input.find_first_of(delimit, begin);
			}
		}
		return out_vec;
	}

    ////////////////////////////////////////////////////////////
    template<typename DelimitType1, typename DelimitType2>
    static std::string& FindAndTrimBlock(std::string& input, DelimitType1 const& begin_pattern, DelimitType2 const& end_pattern, bool ignore_repeated = false) {
        // find begin
        size_t begin = input.find_first_of(begin_pattern);
        if (begin == std::string::npos) return input;

        // find content begin
        size_t content = ignore_repeated ? input.find_first_not_of(begin_pattern, begin) : begin + 1;
        if (content == std::string::npos) return input;

        // find content end
        size_t content_end = input.find_first_of(end_pattern, content);
        if (content_end == std::string::npos) return input;

        // find end
        size_t end = ignore_repeated ? input.find_first_not_of(end_pattern, content_end) : content_end + 1;

        // erase
        input.erase(begin, end == std::string::npos ? end : end - begin);
        return input;
    }

    ////////////////////////////////////////////////////////////
    // replace
    static std::string& ReplaceInplace(std::string& src, std::string const& to_replace,
                                       std::string const& replace_with) {
        for (size_t pos = src.find(to_replace); pos != std::string::npos; pos = src.find(to_replace, pos)) {
            src.replace(pos, to_replace.size(), replace_with);
            pos += replace_with.size();
        }
        return src;
    }

    static std::string Replace(std::string const& src, std::string const& to_replace, std::string const& replace_with) {
        std::string tmp;
        size_t last = 0;
        size_t pos = src.find(to_replace);
        while (pos != std::string::npos) {
            tmp += src.substr(last, pos - last);
            tmp += replace_with;
            last = pos + to_replace.size();
            pos = src.find(to_replace, last);
        }
        tmp += src.substr(last);
        ;
        return tmp;
    }

    ////////////////////////////////////////////////////////////
    // strip
    static std::string& RemoveHeadWhitespace(std::string& input) {
        size_t pos = input.find_first_not_of(WHITE_SPACES);

        if (pos == 0) {
            return input;
        } else if (pos == std::string::npos) {
            input.clear();
        } else {
            input.erase(0, pos);
        }
        return input;
    }

    static std::string& RemoveTailWhitespace(std::string& input) {
        size_t pos = input.find_last_not_of(WHITE_SPACES);

        if (pos == input.size() - 1) {
            return input;
        } else if (pos == std::string::npos) {
            input.clear();
        } else {
            input.erase(pos + 1);
        }
        return input;
    }

    ////////////////////////////////////////////////////////////
    // path related
    static std::string GetDirName(std::string const& input) {
        // remove heading and tailing whitespace
        size_t pos1 = input.find_first_not_of(WHITE_SPACES);
        size_t pos2 = input.find_last_not_of(WHITE_SPACES);

        // remove tailing '/'
        while (input[pos2] == '/' && pos2 != 0) {
            pos2--;
        }

        // find last '/'
        size_t pos = input.find_last_of('/', pos2);

        if (pos == std::string::npos) {
            return std::string(".");
        } else if (pos == pos1) {
            return std::string("/");
        } else {
            return input.substr(pos1, pos - pos1);
        }
        return input;
    }

    static std::string GetBaseName(std::string const& input) {
        // remove heading and tailing whitespace
        size_t pos1 = input.find_first_not_of(WHITE_SPACES);
        size_t pos2 = input.find_last_not_of(WHITE_SPACES);

        // remove tailing '/'
        while (input[pos2] == '/' && pos2 != 0) pos2--;

        // find last '/'
        size_t pos = input.find_last_of('/', pos2);

        if (pos == std::string::npos) {
            return input.substr(pos1, pos2 - pos1 + 1);
        } else {
            return input.substr(pos + 1, pos2 - pos);
        }
        return input;
    }

    static std::string GetSuffix(std::string const& input) {
        std::string tmp = GetBaseName(input);

        size_t pos = tmp.find_last_of('.');

        if (pos == std::string::npos) {
            return std::string("");
        } else {
            return tmp.substr(pos + 1);
        }
        return tmp;
    }

    ////////////////////////////////////////////////////////////
    // hex string
    static std::string BinToString(void const *input, size_t inlen) {
        std::string tmp;
        for (size_t off = 0; off < inlen; off++) {
            unsigned char ch = *((unsigned char *)input + off);
            if (ch < 0x20 || ch > 0x7e)
                tmp.push_back('.');
            else
                tmp.push_back(ch);
        }
        return tmp;
    }

    static std::string BinToHexString(void const *input, size_t inlen) {
        static const char CHAR_TABLE[] = {'0', '1', '2', '3', '4', '5', '6', '7',
                                          '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};

        std::string tmp;
        for (size_t off = 0; off < inlen; off++) {
            tmp.push_back(CHAR_TABLE[(*((unsigned char *)input + off) >> 4) & 0xf]);
            tmp.push_back(CHAR_TABLE[*((unsigned char *)input + off) & 0xf]);
        }
        return tmp;
    }

    static std::string HexStringToBin(char const *input, size_t inlen) {
        std::string tmp;
        tmp.assign(inlen / 2, ' ');

        for (size_t off = 0; off < inlen / 2; off++) {
            switch (input[off * 2]) {
                case '0': tmp[off] = (0x0 << 4) & 0xf0; break;
                case '1': tmp[off] = (0x1 << 4) & 0xf0; break;
                case '2': tmp[off] = (0x2 << 4) & 0xf0; break;
                case '3': tmp[off] = (0x3 << 4) & 0xf0; break;
                case '4': tmp[off] = (0x4 << 4) & 0xf0; break;
                case '5': tmp[off] = (0x5 << 4) & 0xf0; break;
                case '6': tmp[off] = (0x6 << 4) & 0xf0; break;
                case '7': tmp[off] = (0x7 << 4) & 0xf0; break;
                case '8': tmp[off] = (0x8 << 4) & 0xf0; break;
                case '9': tmp[off] = (0x9 << 4) & 0xf0; break;
                case 'a': tmp[off] = (0xa << 4) & 0xf0; break;
                case 'b': tmp[off] = (0xb << 4) & 0xf0; break;
                case 'c': tmp[off] = (0xc << 4) & 0xf0; break;
                case 'd': tmp[off] = (0xd << 4) & 0xf0; break;
                case 'e': tmp[off] = (0xe << 4) & 0xf0; break;
                case 'f': tmp[off] = (0xf << 4) & 0xf0; break;
                case 'A': tmp[off] = (0xa << 4) & 0xf0; break;
                case 'B': tmp[off] = (0xb << 4) & 0xf0; break;
                case 'C': tmp[off] = (0xc << 4) & 0xf0; break;
                case 'D': tmp[off] = (0xd << 4) & 0xf0; break;
                case 'E': tmp[off] = (0xe << 4) & 0xf0; break;
                case 'F': tmp[off] = (0xf << 4) & 0xf0; break;
            }
            switch (input[off * 2 + 1]) {
                case '0': tmp[off] += 0x0; break;
                case '1': tmp[off] += 0x1; break;
                case '2': tmp[off] += 0x2; break;
                case '3': tmp[off] += 0x3; break;
                case '4': tmp[off] += 0x4; break;
                case '5': tmp[off] += 0x5; break;
                case '6': tmp[off] += 0x6; break;
                case '7': tmp[off] += 0x7; break;
                case '8': tmp[off] += 0x8; break;
                case '9': tmp[off] += 0x9; break;
                case 'a': tmp[off] += 0xa; break;
                case 'b': tmp[off] += 0xb; break;
                case 'c': tmp[off] += 0xc; break;
                case 'd': tmp[off] += 0xd; break;
                case 'e': tmp[off] += 0xe; break;
                case 'f': tmp[off] += 0xf; break;
                case 'A': tmp[off] += 0xa; break;
                case 'B': tmp[off] += 0xb; break;
                case 'C': tmp[off] += 0xc; break;
                case 'D': tmp[off] += 0xd; break;
                case 'E': tmp[off] += 0xe; break;
                case 'F': tmp[off] += 0xf; break;
            }
        }
        return tmp;
    }

    static std::string BinToString(std::string const& input) { return BinToString(input.data(), input.size()); }
    static std::string BinToHexString(std::string const& input) { return BinToHexString(input.data(), input.size()); }
    static std::string HexStringToBin(std::string const& input) { return HexStringToBin(input.data(), input.size()); }

    ////////////////////////////////////////////////////////////
    // other
    static std::string Format(char const *fmt, ...) {
        char tmp[8192];

        va_list ap;
        va_start(ap, fmt);
        std::vsprintf(tmp, fmt, ap);
        va_end(ap);

        return std::string(tmp);
    }

    static size_t Utf8Length(std::string const& str) {
        return Utf8Length(str.c_str());
    }

    static size_t Utf8Length(char const* str) {
        size_t length = 0;
        for (size_t i = 0; i < strlen(str); ++i) {
            if ((str[i] & 0xc0) != 0x80) length++;
        }
        return length;
    }

    static std::istream& SafeGetLine(std::istream& is, std::string& t) {
        // The characters in the stream are read one-by-one using a std::streambuf.
        // That is faster than reading them one-by-one using the std::istream.
        // Code that uses streambuf this way must be guarded by a sentry object.
        // The sentry object performs various tasks,
        // such as thread synchronization and updating the stream state.
        t.clear();
        std::istream::sentry se(is, true);
        std::streambuf* sb = is.rdbuf();

        while(true) {
            int c = sb->sbumpc();
            switch (c) {
                case '\n': return is;
                case '\r': if(sb->sgetc() == '\n') sb->sbumpc(); return is;
                case EOF : if(t.empty()) is.setstate(std::ios::eofbit); return is;
                default  : t += (char)c;
            }
        }
    }
};

}  // end of namespace util
}  // end of namespace lsf

// vim:ts=4:sw=4:et:ft=cpp:
