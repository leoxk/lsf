// File:        mysql_conn.hpp
// Description: ---
// Notes:       ---
// Author:      leoxiang <leoxiang727@qq.com>
// Revision:    2011-12-10 by leoxiang

#pragma once

#include <string>
#include "mysql_res.hpp"

namespace SEC {
namespace MYSQL {

class MysqlConn
{
public:
    // error constants
    enum ErrType
    {
        kGood = 0,
        kErrInit,
        kErrConnect,
        kErrQuery,
        kErrStoreRes,
    };

public:
    explicit MysqlConn();
    ~MysqlConn();

    // public functions
	bool Connect(string const & host    = "",
	             string const & user    = "", 
                 string const & passwd  = "", 
		         string const & dbname  = "", 
                 size_t port            = 0,
                 string const & sock    = "",
                 unsigned log flag      = 0);
    void Close();
	bool Execute(string const & query);		
	MysqlRes Query(string const & query);

    // error handle
    err_type & err()        { return _err; }
    string   & err_string() { return _err_string; }

    // operators
    operator bool() const { return !_err; }
    MysqlConn& operator<<(string const & query);
    MysqlConn& operator>>(MysqlRes& res);

private:
    // internal variables
    MYSQL*      _mysql;
    // error handle
    err_type    _err;
    string      _err_string;

}; // end of class MysqlConn 

////////////////////////////////////////////////////////////
// implementation

inline MysqlConn::MysqlConn()
    : _mysql(NULL), _err(kGood), _err_string("")
{ 
    _mysql = mysql_init();
    if (NULL == _mysql)
    {
        _err = kErrInit;
        _err_string = mysql_error(_mysql);
    }
}

inline MysqlConn::~MysqlConn()
{ 
    if (NULL != _mysql)
        mysql_close(_mysql); 
}

inline bool MysqlConn::Connect(string const & host    = "",
	                           string const & user    = "", 
                               string const & passwd  = "", 
		                       string const & dbname  = "", 
                               size_t port            = 0,
                               string const & sock    = "",
                               unsigned log flag      = 0)
{
    // do connect
    if (NULL == mysql_real_connect(_mysql,
                           host.empty()   ? "" : host.c_str(), 
                           user.empty()   ? "" : host.c_str(), 
                           passwd.empty() ? "" : host.c_str(), 
                           dbname.empty() ? "" : host.c_str(), 
                           port,
                           port.empty()   ? "" : host.c_str(), 
                           flag))
    {
        _err = kErrConnect;
        _err_string = mysql_error(_mysql);
        return false;
    }
    // set auto reconnect
    return true;
}

inline void MysqlConn::Close()
{ 
    if (NULL != _mysql)
    {
        mysql_close(_mysql); 
        _mysql = NULL;
    }
}

MysqlConn& MysqlConn::operator<<(string const & query)
{
    if (0 != mysql_real_query(_mysql, query.c_str(), query.size()))
    {
        _err = kErrQuery;
        _err_string = mysql_error(_mysql);
    }
    return *this;
}

MysqlConn& MysqlConn::operator>>(MysqlRes& res)
{
    MYSQL_RES* pres = mysql_store_result(_mysql);
    if (pres == NULL)
    {
        if (0 == mysql_field_count(_mysql))
            _err_string = "Query should not return result";
        else 
            _err_string = mysql_error(_mysql);
        _err = kErrStoreRes;
    }
    else
    {
        res.SetMysqlRes(pres);
    }
    return *this;
}

inline bool MysqlConn::Execute(string const & query)
{
    if (0 != mysql_real_query(_mysql, query.c_str(), query.size()))
    {
        _err = kErrQuery;
        _err_string = mysql_error(_mysql);
        return false;
    }
    return true;
}

inline MysqlRes MysqlConn::Query(string const & query)
{
    if (0 != mysql_real_query(_mysql, query.c_str(), query.size()))
    {
        _err = kErrQuery;
        _err_string = mysql_error(_mysql);
        return MysqlRes();
    }
    // release of MYSQL_RES* should be handled by MysqlSharedRes
    MYSQL_RES* pres = mysql_store_result(_mysql);
    if (pres == NULL)
    {
        if (0 == mysql_field_count(_mysql))
            _err_string = "Query should not return result";
        else 
            _err_string = mysql_error(_mysql);
        _err = kErrStoreRes;
        return MysqlRes();
    }
    return MysqlRes(pres);
}

} // end of namespace MYSQL
} // end of namespace SEC

// vim:ts=4:sw=4:et:ft=cpp:
