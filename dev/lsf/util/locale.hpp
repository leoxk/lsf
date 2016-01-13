// File:        locale.hpp
// Description: ---
// Notes:       ---
// Author:      leoxiang <leoxiang727@qq.com>
// Revision:    2015-08-13 by leoxiang

#pragma once
#include <iconv.h>
#include "lsf/basic/error.hpp"

namespace lsf {
namespace util {

////////////////////////////////////////////////////////////
// Locale
class Locale : lsf::basic::Error {
public:
    static bool ConvertGbkToUtf8(std::string& output) {
        return ConvertCode("GB2312", "UTF-8", output);
    }

    static bool ConvertGbkToUtf8(std::string const& input, std::string& output) {
        return ConvertCode("GB2312", "UTF-8", input, output);
    }

    static bool ConvertUtf8ToGbk(std::string const& input, std::string& output) {
        return ConvertCode("UTF-8", "GB2312", input, output);
    }

private:
    static bool ConvertCode(char const* from_charset, char const* to_charset, std::string& output) {
        return ConvertCode(from_charset, to_charset, output.data(), output.size(), output);
    }

    static bool ConvertCode(char const* from_charset, char const* to_charset, std::string const& input, std::string& output) {
        return ConvertCode(from_charset, to_charset, input.data(), input.size(), output);
    }

    static bool ConvertCode(char const* from_charset, char const* to_charset, char const* inbuf, size_t inlen, std::string& output) {
        // init buffer
        size_t outlen = inlen*4;
        char* outbuf = new char[outlen];

        // do convert
        size_t new_outlen = outlen;
        size_t new_inlen = inlen;
        bool ret = ConvertCode(from_charset, to_charset, (char*)inbuf, &new_inlen, outbuf, &new_outlen);
        if (ret) output.assign(outbuf, outlen - new_outlen);

        // release buffer
        delete[] outbuf;
        return ret;
    }

    static bool ConvertCode(char const* from_charset, char const* to_charset, void* inbuf, size_t* pinlen, void* outbuf, size_t* poutlen) {
        // get convert descriptor
        iconv_t conv_desc = iconv_open(to_charset, from_charset);
        if (conv_desc == iconv_t(-1)) return false;

        // do convert
        char** pinbuf  = (char**)&inbuf;
        char** poutbuf = (char**)&outbuf;
        if (iconv(conv_desc, pinbuf, pinlen , poutbuf, poutlen) == size_t(-1)) {
            iconv_close(conv_desc);
            return false;
        }

        // clear up
        iconv_close(conv_desc);
        return true;
    }
};

} // end namespace util
} // end namespace lsf

// vim:ts=4:sw=4:et:ft=cpp:
