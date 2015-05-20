// File:        mysql_res.hpp
// Description: ---
// Notes:       ---
// Author:      leoxiang <leoxiang727@qq.com>
// Revision:    2011-12-10 by leoxiang

#pragma once

#include <string>
#include "mysql_detail.hpp"
#include "mysql_row.hpp"

namespace SEC {
namespace MYSQL {

class MysqlRes
{
public:
    // typedef 
    typedef MysqlRow *              iterator;
    typedef MysqlRow const *        const_iterator;

public:
    explicit MysqlRes(MYSQL_RES* res = NULL) 
        : _shared_res(res), _err(kGood), _err_string("") { }

    // public functions
    MysqlRow    GetRow(size_t index) const { return MysqlRow(_shared_res, index); }
    //MYSQL_FIELD GetField(size_t index) const;
    size_t   GetRowNum()   const { return mysql_num_rows(_shared_res.Get()); }
    size_t   GetFieldNum() const { return mysql_num_fields(_shared_res.Get()); }
    void SetMysqlRes(MYSQL_RES* res) { _shared_res = detail::MysqlSharedRes(res); }

    // iterator
    // TODO

    // operators
    operator bool() const { return _shared_res.Get() != NULL; }
    MysqlRow operator[] (size_t index) const { return GetRow(index); }
    //MysqlRow operator[] (string const & col_name) const; TODO

private:
    // internal variables
    detail::MysqlSharedRes _shared_res;
}; // end of class MysqlRes

////////////////////////////////////////////////////////////
// implementation

} // end of namespace MYSQL
} // end of namespace SEC

// vim:ts=4:sw=4:et:ft=cpp:
