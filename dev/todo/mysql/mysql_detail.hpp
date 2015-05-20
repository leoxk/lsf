// File:        mysql_detail.hpp
// Description: ---
// Notes:       ---
// Author:      leoxiang <leoxiang727@qq.com>
// Revision:    2011-12-10 by leoxiang

#pragma once

namespace SEC {
namespace MYSQL {
namespace detail {

class MysqlSharedRes
{
public:
    explicit MysqlSharedRes(MYSQL_RES* pres = NULL) 
        : _pres(pres), _pcnt(new size_t(0)) 
    { _RefAdd(); }
    
    MysqlSharedRes(MysqlSharedRes const & rhs) 
        : _pres(rhs._pres), _pcnt(rhs._pcnt)
    { _RefAdd(); }

    ~MysqlSharedRes() { _RefRelease(); }

    MysqlSharedRes& operator=(MysqlSharedRes const & rhs)
    {
        if (_pres == rhs._pres)
            return *this;

        _RefRelease();
        _pcnt = rhs._pcnt;
        _pres = _pres;
        _RefAdd();
        return *this;
    }

    MYSQL_RES* Get() const { return _pres; }

private:

    void _RefAdd()
    { ++*_pcnt; }

    void _RefRelease() 
    { 
        if (--*_pcnt == 0) { 
            delete _pcnt; 
            if (_pres != NULL) mysql_free_result(_pres); 
        } 
    }

    MYSQL_RES*  _pres;
    size_t*     _pcnt;

}; // end of class MysqlSharedRes

} // end of namespace detail
} // end of namespace MYSQL
} // end of namespace SEC

// vim:ts=4:sw=4:et:ft=cpp:
