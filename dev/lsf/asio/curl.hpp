// File:        curl.hpp
// Description: ---
// Notes:       ---
// Author:      leoxiang <leoxiang727@qq.com>
// Revision:    2015-08-25 by leoxiang

#pragma once
#include <exception>
#include <memory>
#include <functional>
#include <set>
#include <map>
#include "lsf/basic/unit_test.hpp"
#include "lsf/basic/error.hpp"
#include "lsf/basic/macro.hpp"
#include "lsf/basic/singleton.hpp"
#include "lsf/basic/scope_exit.hpp"
#include "lsf/util/log.hpp"
#include "lsf/asio/async.hpp"
#include "curl/curl.h"

namespace lsf {
namespace asio {

////////////////////////////////////////////////////////////
// Curl
class Curl {
public:
    static const size_t DEF_LOW_SPEED_TIME = 3; // seconds
    static const size_t DEL_LOW_SPEED_LIMIT = 10; // bytes/s
    friend class CurlMulti;

    static size_t WriteToStringCallback(void* ptr, size_t size, size_t nmemb, void* puser) {
        Curl* pcurl = (Curl*)puser;
        pcurl->_output.append((char const*)ptr, size*nmemb);
        return size*nmemb;
    }

    static int DebugCallback(CURL* pcurl, curl_infotype type, char* data, size_t size, void* puser) {
        // ignore tailing cr
        if (size > 0) size--;
        switch (type) {
            case CURLINFO_TEXT:         LSF_LOG_INF("[curl text]: %.*s", size, data); break;
            case CURLINFO_HEADER_IN:    LSF_LOG_INF("[curl header in]: %.*s", size, data); break;
            case CURLINFO_HEADER_OUT:   LSF_LOG_INF("[curl header out]: %.*s", size, data); break;
            case CURLINFO_DATA_IN:      LSF_LOG_INF("[curl data in]: %.*s", size, data); break;
            case CURLINFO_DATA_OUT:     LSF_LOG_INF("[curl data out]: %.*s", size, data); break;
            case CURLINFO_SSL_DATA_IN:  LSF_LOG_INF("[curl ssl data in]: %.*s", size, data); break;
            case CURLINFO_SSL_DATA_OUT: LSF_LOG_INF("[curl ssl data out]: %.*s", size, data); break;
            case CURLINFO_END: break;
        }
        return 0;
    }

    static int ProgressCallback(void* puser, double dltotal, double dlnow, double ultotal, double ulnow) {
        // Curl* pcurl = (Curl*)puser;
        return 0;
    }

    static Curl* GetCurlFromNative(CURL* pnative) {
        Curl* pcurl;
        return curl_easy_getinfo(pnative, CURLINFO_PRIVATE, &pcurl) == CURLE_OK ? pcurl : nullptr;
    }

    std::string UrlEscape(std::string const& input) {
        std::string tmp = curl_easy_escape(_handle, input.c_str(), input.size());
        return tmp;
    }

public:
    // constructor
    Curl() : _handle(curl_easy_init()) {
        if (!*this) return;
        // forbid progress meter
        SetOpt(CURLOPT_NOPROGRESS, 1);
        // init private
        SetOpt(CURLOPT_PRIVATE, this);
    }
    ~Curl() { if (*this) curl_easy_cleanup(_handle); }

    explicit operator bool() const { return _handle != nullptr; }
    bool operator<(Curl const& rhs) const { return _handle < rhs._handle; }

    std::string const& Output() const { return _output; }
    std::string&& MoveOutput() { return std::move(_output); }
    std::string ErrString() const { return curl_easy_strerror(_error); }
    bool Success() const { return _error == CURLE_OK; }
    CURL* NativeHandle() const { return _handle; }

    // common function
    bool Perform() {
        _output.clear();
        return (_error = curl_easy_perform(_handle)) == CURLE_OK;
    }

    // get option
    template<typename ParamType>
    bool GetInfo(CURLINFO info, ParamType param) {
        return curl_easy_getinfo(_handle, info, param) == CURLE_OK;
    }

    std::string EffectiveUrl() {
        char* purl;
        if (!GetInfo(CURLINFO_EFFECTIVE_URL, &purl)) return "";
        return purl;
    }

    // set option
    void Reset() { curl_easy_reset(_handle); }

    template<typename ParamType>
    bool SetOpt(CURLoption option, ParamType param) {
        return (_error = curl_easy_setopt(_handle, option, param)) == CURLE_OK;
    }

    bool SetUrl(std::string const& string) { return SetOpt(CURLOPT_URL, string.c_str()); }
    bool SetUrl(char const* char_string) { return SetOpt(CURLOPT_URL, char_string); }

    bool SetPostData(std::string const& string) {
        _http_body = string;
        if (!SetOpt(CURLOPT_POSTFIELDSIZE, _http_body.size())) return false;
        if (!SetOpt(CURLOPT_POSTFIELDS, _http_body.data())) return false;
        return true;
    }

    bool SetOutputToString() {
        if (!SetOpt(CURLOPT_WRITEFUNCTION, WriteToStringCallback)) return false;
        if (!SetOpt(CURLOPT_WRITEDATA, this)) return false;
        return true;
    }

    bool SetVerbose() {
        if (!SetOpt(CURLOPT_VERBOSE, 1)) return false;
        if (!SetOpt(CURLOPT_DEBUGFUNCTION, DebugCallback)) return false;
        return true;
    }

    bool SetProgress() {
        if (!SetOpt(CURLOPT_NOPROGRESS, 0)) return false;
        if (!SetOpt(CURLOPT_PROGRESSFUNCTION, ProgressCallback)) return false;
        if (!SetOpt(CURLOPT_PROGRESSDATA, this)) return false;
        return true;
    }

    bool SetLowSpeedAbort(size_t low_speed_time = DEF_LOW_SPEED_TIME, size_t low_speed_limit = DEL_LOW_SPEED_LIMIT) {
        if (!SetOpt(CURLOPT_LOW_SPEED_TIME, low_speed_time)) return false;
        if (!SetOpt(CURLOPT_LOW_SPEED_LIMIT, low_speed_limit)) return false;
        return true;
    }

    // use milli seconds
    bool SetTimeout(size_t timeout) {
        if (!SetOpt(CURLOPT_TIMEOUT_MS, timeout)) return false;
        if (!SetOpt(CURLOPT_CONNECTTIMEOUT_MS, timeout)) return false;
        return true;
    }

private:
    CURL* _handle = nullptr;
    std::string _output;
    std::string _http_body;
    CURLcode _error = CURLE_OK;
};

////////////////////////////////////////////////////////////
//
class SharedCurl : public std::shared_ptr<Curl> {
public:
    using base_type = std::shared_ptr<Curl>;
    SharedCurl() : base_type(new Curl) {}
};

} // end namespace asio
} // end namespace lsf

////////////////////////////////////////////////////////////
// define hash function
namespace std {
template<>
struct hash<lsf::asio::Curl> {
    size_t operator()(lsf::asio::Curl const& curl) const {
        return std::hash<char*>()((char*)curl.NativeHandle());
    }
};
} // end namespace std

// vim:ts=4:sw=4:et:ft=cpp:
