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
    static const size_t DEL_TIMEOUT = 10; // seconds
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
        // forbid signal for thread safty
        SetOpt(CURLOPT_NOSIGNAL, 1);
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

    bool SetTimeout(size_t timeout = DEL_TIMEOUT) {
        if (!SetOpt(CURLOPT_CONNECTTIMEOUT, timeout)) return false;
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

////////////////////////////////////////////////////////////
// CurlMulti
class CurlMulti : public lsf::basic::Singleton<CurlMulti>, public lsf::basic::Error {
public:
    using finish_callback_type = std::function<void(SharedCurl)>;

    // add timer callback
    static void AddTimerCallback(CURLM* multi_handle, long timeout_milli, void* puser) {
        // just invoke timeout immediately
        if (timeout_milli <= 0) timeout_milli = 1;

        // add new timer
        int timer_fd = IOService::Instance()->AsyncAddTimerOnce(timeout_milli, [](int timer_fd) {
            // remove timer fd
            CurlMulti::Instance()->_timer_set.erase(timer_fd);

            // notify action
            int running_handles = 0;
            auto ret = curl_multi_socket_action(CurlMulti::Instance()->_handle, CURL_SOCKET_TIMEOUT, 0, &running_handles);
            if (ret != CURLM_OK) {
                LSF_LOG_ERR("socket action failed, fd=%u, %s", timer_fd, curl_multi_strerror(ret));
            }

            // check handle
            CheckMultiInfo();
        });

        // add to set
        if (timer_fd) CurlMulti::Instance()->_timer_set.insert(timer_fd);
    }

    // handle socket callback
    static int HandleSocketCallback(CURL* easy_handle, curl_socket_t sock, int action, void* puser, void* psock) {
        // handle action
        switch (action) {
            case CURL_POLL_IN:
            case CURL_POLL_OUT: {
                int flag = (action == CURL_POLL_IN ? IOService::FLAG_READ : IOService::FLAG_WRITE);
                IOService::Instance()->AsyncRawCallback(sock, flag, [](int sockfd, int flags) {
                    // init curl flags
                    int curl_flags = 0;
                    if (flags & IOService::FLAG_READ)  curl_flags |= CURL_CSELECT_IN;
                    if (flags & IOService::FLAG_WRITE) curl_flags |= CURL_CSELECT_OUT;
                    if (flags & IOService::FLAG_ERR)   curl_flags |= CURL_CSELECT_ERR;

                    // notify action
                    int running_handles = 0;
                     auto ret = curl_multi_socket_action(CurlMulti::Instance()->_handle, sockfd, curl_flags, &running_handles);
                    if (ret != CURLM_OK) {
                        LSF_LOG_ERR("socket action failed, fd=%u, %s", sockfd, curl_multi_strerror(ret));
                    }

                    // check handle
                    CheckMultiInfo();

                    // if all done, kill all timer
                    if (running_handles <= 0) {
                        for (auto timer_fd : CurlMulti::Instance()->_timer_set) {
                            IOService::Instance()->AsyncCloseTimer(timer_fd);
                        }
                        CurlMulti::Instance()->_timer_set.clear();
                    }
                });
                break;
            }

            case CURL_POLL_REMOVE: {
                IOService::Instance()->AsyncCancel(sock);
                break;
            }
        }
        return 0;
    }

    static void CheckMultiInfo() {
        // traverse all curl
        int pending;
        CURLMsg* pmsg = nullptr;
        while ((pmsg = curl_multi_info_read(CurlMulti::Instance()->_handle, &pending))) {
            switch (pmsg->msg){
                case CURLMSG_DONE: {
                    // release from multi
                    curl_multi_remove_handle(CurlMulti::Instance()->_handle, pmsg->easy_handle);

                    // get curl instance
                    auto& curl_map = CurlMulti::Instance()->_curl_map;
                    auto iter = curl_map.find(pmsg->easy_handle);
                    if (iter == curl_map.end()) {
                        LSF_LOG_ERR("cant get curl when process done, handle=%x", pmsg->easy_handle);
                        continue;
                    }

                    // set result
                    iter->second.first->_error = pmsg->data.result;

                    // callback
                    if (iter->second.second) iter->second.second(iter->second.first);

                    // release obj
                    curl_map.erase(iter);
                }
                break;

                default: break;
            }
        }
    }

public:
    CurlMulti() {}
    ~CurlMulti() {if (*this) curl_multi_cleanup(_handle); }
    explicit operator bool() const { return _handle != nullptr; }

    bool Init() {
        // check already init
        if (_handle) return true;

        // init global handle
        if (curl_global_init(CURL_GLOBAL_ALL) != 0) {
            SetErrString(LSF_DEBUG_INFO);
            return false;
        }

        // check async dns:
        // because we forbid signal in Curl constructor
        // so we must make sure libcurl support async dns lookup
        // otherwise dns lookup may last forever
        // here we just give errinfo, do not force error
        auto* pinfo = curl_version_info(CURLVERSION_NOW);
        if (!(pinfo->features & CURL_VERSION_ASYNCHDNS)) {
            LSF_LOG_ERR("libcurl do not support async dns, please libcurl with c-ares");
            // SetErrString(LSF_DEBUG_INFO + std::string(" Not Support Async DNS"));
            // return false;
        }

        // init multi handle
        if ((_handle = curl_multi_init()) == nullptr) {
            SetErrString(LSF_DEBUG_INFO);
            return false;
        }

        // set callback
        curl_multi_setopt(_handle, CURLMOPT_SOCKETFUNCTION, HandleSocketCallback);
        curl_multi_setopt(_handle, CURLMOPT_TIMERFUNCTION, AddTimerCallback);
        return true;
    }

    template<typename HandlerType = std::nullptr_t>
    bool AddCurl(SharedCurl shared_curl, HandlerType&& handler = nullptr) {
        // check input
        if (!shared_curl) return false;

        // set callbacks
        shared_curl->SetOutputToString();

        // add to map
        _curl_map[shared_curl->NativeHandle()] = std::make_pair(shared_curl, std::forward<HandlerType>(handler));

        // add to handle
        curl_multi_add_handle(_handle, shared_curl->NativeHandle());

        return true;
    }

    void RemoveCurl(SharedCurl shared_curl) {
        _curl_map.erase(shared_curl->NativeHandle());
    }

    std::set<int> const& TimerSet() const { return _timer_set; }
    std::map<CURL*,std::pair<SharedCurl,finish_callback_type>> const& CurlMap() const { return _curl_map; }

private:
    CURLM* _handle = nullptr;
    std::set<int> _timer_set;
    std::map<CURL*,std::pair<SharedCurl,finish_callback_type>> _curl_map;
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
