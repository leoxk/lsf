// File:        words_filter.hpp
// Description: ---
// Notes:       ---
// Author:      leoxiang <leoxiang727@qq.com>
// Revision:    2015-09-15 by leoxiang

#pragma once
#include <map>
#include <cstring>
#include "lsf/basic/singleton.hpp"
#include "lsf/util/string_ext.hpp"

namespace lsf {
namespace util {

class WordsFilter : public lsf::basic::Singleton<WordsFilter> {
public:
    using map_type = std::multimap<char,std::string>;
    constexpr static char DEF_SUB_STRING = '*';

public:
    void AddWord(std::string const& word) {
        if (!word.empty()) _map.emplace(word.front(), word);
    }

    void Clear() {
        _map.clear();
    }

    bool CheckHasWord(std::string const& sentence) {
        // traverse all char
        for (size_t i = 0; i < sentence.length(); ++i) {
            // check word exist
            auto range = _map.equal_range(sentence[i]);
            if (range.first == range.second) continue;

            // travers all words
            for (auto it = range.first; it != range.second; ++it) {
                // check enough length
                std::string const& word = it->second;
                if (sentence.length() - i < word.length()) continue;

                // compare
                if (::memcmp(sentence.data()+i, word.data(), word.length()) == 0) return true;
            }
        }

        return false;
    }

    bool SubstitueInplace(std::string& sentence, char subchar = DEF_SUB_STRING) {
        // traverse all char
        bool find = false;
        for (size_t i = 0; i < sentence.length(); ++i) {
            // check word exist
            auto range = _map.equal_range(sentence[i]);
            if (range.first == range.second) continue;

            // travers all words
            for (auto it = range.first; it != range.second; ++it) {
                // check enough length
                std::string const& word = it->second;
                if (sentence.length() - i < word.length()) continue;

                // compare and substitue
                if (::memcmp(sentence.data()+i, word.data(), word.length()) == 0) {
                    size_t word_length = lsf::util::StringExt::Utf8Length(word);
                    sentence.replace(i, word.length(), word_length, subchar);
                    find = true;
                }
            }
        }

        return find;
    }

    std::string Substitue(std::string const& sentence, char subchar = DEF_SUB_STRING) {
        std::string tmp = sentence;
        SubstitueInplace(tmp, subchar);
        return tmp;
    }

private:
    map_type _map;

};

} // end namespace util
} // end namespace lsf


// vim:ts=4:sw=4:et:ft=cpp:
