// File:        two_dimensional_table.hpp
// Description: ---
// Notes:       ---
// Author:      leoxiang <leoxiang727@qq.com>
// Revision:    2015-11-09 by leoxiang

#pragma once
#include <vector>
#include <string>
#include <fstream>
#include <stdexcept>
#include <sstream>
#include "lsf/util/string_ext.hpp"
#include "lsf/basic/error.hpp"
#include "lsf/basic/type_cast.hpp"

namespace lsf {
namespace algorithm {

template<typename ElemType>
class TwoDimensionalTable :
    public std::vector<std::vector<ElemType>>,
    public lsf::basic::Error {
public:
    using base_type = std::vector<std::vector<ElemType>>;
    static const int FIELD_DELIMIT = ',';

public:
    // util func
    std::string ToString(size_t off) {
        auto& vec = base_type::at(off);
        std::ostringstream oss;
        for (auto& field : vec) {
            oss << field << ",";
        }
        return oss.str();
    }

    ElemType Get(size_t voff, size_t hoff) {
        if (voff >= base_type::size()) return ElemType();
        if (hoff >= base_type::at(voff).size()) return ElemType();
        return base_type::at(voff).at(hoff);
    }

    // process config
    bool ParseFromFile(std::string const& file_name) {
        // open file
        std::fstream fs;
        fs.open(file_name, std::ios::in);
        if (!fs.is_open()) {
            SetErrString("open file failed: " + file_name);
            return false;
        }

        // clear table
        base_type::clear();

        // process every line
        std::string line;
        while (lsf::util::StringExt::SafeGetLine(fs, line)) {
            // ignore empty
            if (line.empty()) continue;

            // add new line
            base_type::emplace_back();
            auto& table_line = *base_type::rbegin();

            // traverse every field
            size_t pos = 0;
            bool has_real_field = false;
            do  {
                std::string field = lsf::util::StringExt::SplitGetNext(line, (char)FIELD_DELIMIT, pos);
                lsf::util::StringExt::RemoveHeadWhitespace(field);
                lsf::util::StringExt::RemoveTailWhitespace(field);
                // handle throws
                try {
                    table_line.push_back(lsf::basic::TypeCast<ElemType>(field));
                }
                catch (std::invalid_argument const& ia) {
                    table_line.emplace_back();
                }
                if (!field.empty()) has_real_field = true;
            }
            while (pos != std::string::npos);

            // if empty line just ignore
            if (!has_real_field) {
                base_type::erase(base_type::end()-1);
                continue;
            }
        }

        return true;
    }
};

} // end namespace algorithm
} // end namespace lsf

// vim:ts=4:sw=4:et:ft=cpp:
