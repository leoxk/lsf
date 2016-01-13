// File:        config.hpp
// Description: ---
// Notes:       ---
// Author:      leoxiang <leoxiang727@qq.com>
// Revision:    2011-09-16 by leoxiang

#pragma once

#include <map>
#include <string>
#include <fstream>
#include <sstream>
#include <cstring>
#include "lsf/basic/macro.hpp"
#include "lsf/basic/type_cast.hpp"
#include "lsf/basic/singleton.hpp"
#include "lsf/basic/error.hpp"
#include "lsf/util/string_ext.hpp"

namespace lsf {
namespace util {

////////////////////////////////////////////////////////////
// Config
class Config : public lsf::basic::Error {
public:
    using map_type = std::map<std::string, std::string>;
    using store_type = std::map<std::string, map_type>;

    constexpr static const char * DEF_DELIMIT = " \t=";
    constexpr static const char * DEF_COMMENT = "#";
    constexpr static const char * DEF_MODULE_NAME = "__anonymous__";

public:
    Config(std::string const& filename = "", char const * delimit = DEF_DELIMIT, char const * comment = DEF_COMMENT)
        : _delimit(delimit), _comment(comment) {
        if (!filename.empty()) ParseFromFile(filename);
    }

    // member funcs
    bool ParseFromFile(std::string const& filename) {
        std::fstream fs;
        fs.open(filename.c_str(), std::ios::in);

        if (!fs.is_open()) {
            SetErrString(LSF_DEBUG_INFO + SysErrString());
            return false;
        }
        fs >> *this;
        return true;
    }

    bool ParseFromString(std::string const& input) {
        std::istringstream input_stream(input + "\n");
        input_stream >> *this;
        return true;
    }

    std::string Get(std::string const& module, std::string const& key) const {
        store_type::const_iterator module_iter = _data.find(module);
        if (module_iter == _data.end()) return "";

        map_type::const_iterator val_iter = module_iter->second.find(key);
        if (val_iter == module_iter->second.end()) return "";

        return val_iter->second;
    }

    std::string Get(std::string const& key) const { return Get(DEF_MODULE_NAME, key); }

    template <typename OutType>
    OutType Get(std::string const& module, std::string const& key) {
        return basic::TypeCast<OutType>(Get(module, key));
    }

    template <typename OutType>
    OutType Get(std::string const& key) {
        return Get<OutType>(DEF_MODULE_NAME, key);
    }

    // accessor
    void SetDelimit(std::string const& delimit) { _delimit = delimit; }
    void SetComment(std::string const& comment) { _comment = comment; }

    size_t size() const { return _size; }

    // other func
    void PrintAll(std::ostream& os) {
        for (store_type::const_iterator map_iter = _data.begin(); map_iter != _data.end(); map_iter++) {
            for (map_type::const_iterator val_iter = map_iter->second.begin(); val_iter != map_iter->second.end();
                 val_iter++) {
                os << map_iter->first << "\t\t" << val_iter->first << "\t\t" << val_iter->second << std::endl;
            }
        }
    }

    void Clear() {
        _size = 0;
        _delimit = DEF_DELIMIT;
        _comment = DEF_COMMENT;
        _data.clear();
    }

    friend std::istream& operator>>(std::istream& is, Config& cf);

private:
    // internal variables
    size_t _size = 0;
    std::string _delimit = DEF_DELIMIT;
    std::string _comment = DEF_COMMENT;
    store_type _data;
};

////////////////////////////////////////////////////////////
// parse config from is
inline std::istream& operator>>(std::istream& is, Config& cf) {
    std::string line;
    std::string module = Config::DEF_MODULE_NAME;

    for (; !is.eof(); std::getline(is, line)) {
        size_t pos1, pos2;

        // find and remove comments
        if ((pos1 = line.find(cf._comment)) != std::string::npos) line.erase(pos1);

        // remove heading and tailing white spaces
        util::StringExt::RemoveHeadWhitespace(line);
        util::StringExt::RemoveTailWhitespace(line);

        // ignore empty line
        if (line.empty()) continue;

        // case1, this is a module declare
        if (*line.begin() == '[' && *(--line.end()) == ']') {
            module = std::string(line.begin() + 1, line.end() - 1);
            continue;
        }

        // case2, this is a config
        pos1 = line.find_first_of(cf._delimit);
        pos2 = line.find_last_of(cf._delimit);
        if (pos1 == std::string::npos || pos1 == 0 || pos2 == std::string::npos || pos2 == line.size() - 1) {
            continue;
        }
        cf._data[module][line.substr(0, pos1)] = line.substr(pos2 + 1);

        cf._size++;
    }

    return is;
};

////////////////////////////////////////////////////////////
// Singleton Config, provide macros for convient access
class SingleConfig : public Config, public lsf::basic::Singleton<SingleConfig> {};

}  // end of namespace util
}  // end of namespace lsf

// vim:ts=4:sw=4:et:ft=cpp:
