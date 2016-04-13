// File:        curl_multi.hpp
// Description: ---
// Notes:       ---
// Author:      leoxiang <leoxiang727@qq.com>
// Revision:    2016-03-31 by leoxiang

#pragma once
#include "lsf/asio/curl.hpp"
#include "lsf/asio/async.hpp"

namespace lsf {
namespace asio {

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

            // if all done, kill all timer
            if (running_handles <= 0) {
                for (auto timer_fd : CurlMulti::Instance()->_timer_set) {
                    IOService::Instance()->AsyncCloseTimer(timer_fd);
                }
                CurlMulti::Instance()->_timer_set.clear();
            }

            // check handle
            CheckMultiInfo();
        });

        // add to set
        if (timer_fd != -1) CurlMulti::Instance()->_timer_set.insert(timer_fd);
    }

    // handle socket callback
    static int HandleSocketCallback(CURL* easy_handle, curl_socket_t sock, int action, void* puser, void* psock) {
        // handle action
        switch (action) {
            case CURL_POLL_IN:
            case CURL_POLL_OUT:
            case CURL_POLL_INOUT:
            {
                // here use epoll ET mode for CPU optimise
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

                    // if all done, kill all timer
                    if (running_handles <= 0) {
                        for (auto timer_fd : CurlMulti::Instance()->_timer_set) {
                            IOService::Instance()->AsyncCloseTimer(timer_fd);
                        }
                        CurlMulti::Instance()->_timer_set.clear();
                    }

                    // check handle
                    CheckMultiInfo();
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

        // set string output
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

// vim:ts=4:sw=4:et:ft=cpp:
