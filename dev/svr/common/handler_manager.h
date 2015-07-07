// File:        handler_manager.h
// Description: ---
// Notes:       ---
// Author:      leoxiang <leoxiang727@qq.com>
// Revision:    2015-07-07 by leoxiang

#pragma once
#include "svr/common/common_header.h"

////////////////////////////////////////////////////////////
// HandlerManager
class HandlerManager : lsf::basic::Singleton<HandlerManager> {
public:
    static const size_t DEF_MAX_HANDLER_NUM = 5000;

private:
    
};

// vim:ts=4:sw=4:et:ft=cpp:
