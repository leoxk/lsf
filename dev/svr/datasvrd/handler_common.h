// File:        handler_common.h
// Description: ---
// Notes:       ---
// Author:      leoxiang <leoxiang727@qq.com>
// Revision:    2015-08-07 by leoxiang

#include "svr/common/basic_handler.h"

////////////////////////////////////////////////////////////
class QueryDataHandler : public BasicHandler {
public:
    virtual data::ENSessionState OnServerRequest(Session& session);
};

////////////////////////////////////////////////////////////
class UpdateDataHandler : public BasicHandler {
public:
    virtual data::ENSessionState OnServerRequest(Session& session);
};

////////////////////////////////////////////////////////////
class DeleteDataHandler : public BasicHandler {
public:
    virtual data::ENSessionState OnServerRequest(Session& session);
};

////////////////////////////////////////////////////////////
class InsertDataHandler : public BasicHandler {
public:
    virtual data::ENSessionState OnServerRequest(Session& session);
};

////////////////////////////////////////////////////////////
class UnlockDataHandler : public BasicHandler {
public:
    virtual data::ENSessionState OnServerRequest(Session& session);
};

// vim:ts=4:sw=4:et:ft=cpp:
