// File:        curl_thread.hpp
// Description: ---
// Notes:       ---
// Author:      leoxiang <leoxiang727@qq.com>
// Revision:    2016-03-31 by leoxiang

#pragma once
#include <thread>
#include <mutex>
#include <deque>
#include <condition_variable>
#include <pthread.h>
#include "lsf/asio/curl.hpp"

namespace lsf {
namespace asio {

////////////////////////////////////////////////////////////
// CurlThread
class CurlThread : public lsf::basic::Singleton<CurlThread>, public lsf::basic::Error {
public:
    static const size_t DEF_THREAD_COUNT = 10;
    using finish_callback_type = std::function<void(SharedCurl)>;

public:
    bool Init(size_t thread_count = DEF_THREAD_COUNT) {
        // check already init
        if (_is_init) return true;

        // check async dns:
        // because we forbid signal in Curl constructor
        // so we must make sure libcurl support async dns lookup
        // otherwise dns lookup may last forever
        auto* pinfo = curl_version_info(CURLVERSION_NOW);
        if (!(pinfo->features & CURL_VERSION_ASYNCHDNS)) {
            SetErrString(LSF_DEBUG_INFO + std::string(" Not Support Async DNS"));
            return false;
        }

        // create thread
        for (size_t i = 0; i < thread_count; ++i) {
            std::thread t(&CurlThread::ProcessWorker, this);
            t.detach();
        }

        // register process at tick
        IOService::Instance()->RegisterTickFunction(std::bind(&CurlThread::ProcessOnTick, this));

        // set status and return
        _is_init = true;
        return true;
    }

    template<typename HandlerType = std::nullptr_t>
    bool AddCurl(SharedCurl shared_curl, HandlerType&& handler = nullptr) {
        // check input
        if (!shared_curl) return false;

        // set string output
        shared_curl->SetOutputToString();

        // forbid signal for thread safty
        shared_curl->SetOpt(CURLOPT_NOSIGNAL, 1);

        // add to map
        _func_map[shared_curl->NativeHandle()] =std::forward<HandlerType>(handler);

        // add to input list
        std::unique_lock<std::mutex> input_lck(_input_mtx);
        _input_list.push_back(shared_curl);

        return true;
    }

    void ProcessOnTick() {
        // check input
        if (!_input_list.empty()) {
            // notify
            std::unique_lock<std::mutex> input_lck(_input_mtx);
            _input_cv.notify_one();
        }

        // check output
        if (!_output_list.empty()) {
            // try lock
            std::unique_lock<std::mutex> lck(_output_mtx, std::defer_lock);
            if (!lck.try_lock()) return;

            // copy vector list
            auto list = _output_list;
            _output_list.clear();
            lck.unlock();

            // check output list and trigger callback
            for (auto shared_curl : list) {
                auto iter = _func_map.find(shared_curl->NativeHandle());
                if (iter == _func_map.end()) continue;

                // trigger callback
                if (iter->second) iter->second(shared_curl);

                // erase func
                _func_map.erase(iter);
            }
        }
    }

    ////////////////////////////////////////////////////////////
    void ProcessWorker() {
        // init lock
        std::unique_lock<std::mutex> input_lck(_input_mtx);

        // infinite loop to process curl
        while (true) {
            // wait input event
            _input_cv.wait(input_lck);

            // check input not empty
            if (_input_list.empty()) continue;

            // get one input share curl request
            auto shared_curl = _input_list.front();
            _input_list.pop_front();

            // process
            shared_curl->Perform();

            // lock the output and insert result
            std::unique_lock<std::mutex> output_lck(_output_mtx);
            _output_list.push_back(shared_curl);
        }
    }

    std::map<CURL*, finish_callback_type> const& GetFuncMap() const { return _func_map; }
    std::deque<SharedCurl> const& GetInputList() const { return _input_list; }
    std::deque<SharedCurl> const& GetOutputList() const { return _output_list; }

private:
    std::mutex _input_mtx, _output_mtx;
    std::condition_variable _input_cv;
    std::deque<SharedCurl> _input_list, _output_list;
    std::map<CURL*, finish_callback_type> _func_map;
    bool _is_init = false;
};

} // end namespace asio
} // end namespace lsf

// vim:ts=4:sw=4:et:ft=cpp:
