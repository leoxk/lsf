// File:        handler_common.h
// Description: ---
// Notes:       ---
// Author:      leoxiang <leoxiang727@qq.com>
// Revision:    2015-07-10 by leoxiang

#include "svr/common/basic_handler.h"

////////////////////////////////////////////////////////////
// QuickStartHandler
class QuickStartHandler : public BasicHandler {
public:
    virtual data::ENSessionState OnClientMessage(Session& session);
};

// vim:ts=4:sw=4:et:ft=cpp:
