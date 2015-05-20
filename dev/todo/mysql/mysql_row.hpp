// File:        mysql_row.hpp
// Description: ---
// Notes:       ---
// Author:      leoxiang <leoxiang727@qq.com>
// Revision:    2011-12-10 by leoxiang

#pragma once

#include <string>
#include "mysql_detail.hpp"

namespace SEC {
namespace MYSQL {

class MysqlRow
{
public:
    explicit MysqlRow(detail::MysqlSharedRes const & shared_res, size_t row_idx = 0) 
       : _shared_res(shared_res), _row_idx(row_idx) { }
    
    // public functions

private:
    // internal variables
    detail::MysqlSharedRes _shared_res;
    size_t      _row_idx;
}; // end of MysqlRow

} // end of namespace MYSQL
} // end of namespace SEC

// vim:ts=4:sw=4:et:ft=cpp:
