// File:        read_data_buffer.hpp
// Description: ---
// Notes:       ---
// Author:      leoxiang <leoxiang727@qq.com>
// Revision:    2015-07-28 by leoxiang

#pragma once
#include <unordered_map>
#include "lsf/basic/singleton.hpp"

namespace lsf {
namespace asio {
namespace async {

class ReadDataBuffer : public lsf::basic::Singleton<ReadDataBuffer> {
public:
    using data_map_type = std::unordered_map<int,std::string>;

public:
    void AppendUnusedData(int sockfd, std::string const& data) {
        _data_map[sockfd].append(data);
    }

    void AppendUnusedData(int sockfd, char const* data, size_t len) {
        _data_map[sockfd].append(data, len);
    }

    void CheckUnusedData(int sockfd, std::string& data) {
        // check exist
        auto iter = _data_map.find(sockfd);
        if (iter == _data_map.end()) return;

        // assign
        data.insert(0, iter->second);

        // clear
        _data_map.erase(iter);
    }

    void ClearUnusedData(int sockfd) {
        _data_map.erase(sockfd);
    }

private:
    data_map_type _data_map;
};

} // end namespace async
} // end namespace asio
} // end namespace lsf

// vim:ts=4:sw=4:et:ft=cpp:
