// File:        handler_game.h
// Description: ---
// Notes:       ---
// Author:      leoxiang <leoxiang727@qq.com>
// Revision:    2015-07-20 by leoxiang

#include "svr/common/basic_handler.h"

////////////////////////////////////////////////////////////
class ReserveTableSeatHandler : public BasicHandler {
public:
    virtual data::ENSessionState OnClientRequest(Session& session);
};

////////////////////////////////////////////////////////////
class UnreserveTableSeatHandler : public BasicHandler{
public:
    virtual data::ENSessionState OnClientRequest(Session& session);
};

////////////////////////////////////////////////////////////
class EnterTableHandler : public BasicHandler {
public:
    virtual data::ENSessionState OnClientRequest(Session& session);
};

////////////////////////////////////////////////////////////
class ExitTableHandler : public BasicHandler {
public:
    virtual data::ENSessionState OnClientRequest(Session& session);
    virtual data::ENSessionState OnHttpSvrMessage(Session& session);
};

////////////////////////////////////////////////////////////
class SitDownHandler : public BasicHandler {
public:
    virtual data::ENSessionState OnClientRequest(Session& session);
    virtual data::ENSessionState OnHttpSvrMessage(Session& session);
};

////////////////////////////////////////////////////////////
class StandUpHandler : public BasicHandler {
public:
    virtual data::ENSessionState OnClientRequest(Session& session);
    virtual data::ENSessionState OnHttpSvrMessage(Session& session);
};

////////////////////////////////////////////////////////////
class InternalStandUpHandler : public BasicHandler {
public:
    virtual data::ENSessionState OnServerRequest(Session& session);
    virtual data::ENSessionState OnHttpSvrMessage(Session& session);
};
////////////////////////////////////////////////////////////
class InternalAsyncPlayerExitTableHandler : public InternalStandUpHandler {
public:
    virtual data::ENSessionState OnServerRequest(Session& session);
};

////////////////////////////////////////////////////////////
class AnimationFinishHandler : public BasicHandler {
public:
    virtual data::ENSessionState OnClientRequest(Session& session);
};

////////////////////////////////////////////////////////////
class DoActionHandler : public BasicHandler {
public:
    virtual data::ENSessionState OnClientRequest(Session& session);
};

////////////////////////////////////////////////////////////
class SetPlayerStatusHandler : public BasicHandler {
public:
    virtual data::ENSessionState OnClientRequest(Session& session);
};

////////////////////////////////////////////////////////////
class ChatHandler : public BasicHandler {
public:
    static const size_t MAX_CHAT_LEN = 180;
    virtual data::ENSessionState OnClientRequest(Session& session);
};

////////////////////////////////////////////////////////////
class InternalTableRunStateMachine : public BasicHandler {
public:
    virtual data::ENSessionState OnServerRequest(Session& session);
};

////////////////////////////////////////////////////////////
class ExchangeTableIndexHandler : public BasicHandler {
public:
    virtual data::ENSessionState OnClientRequest(Session& session);
};

////////////////////////////////////////////////////////////
class PlayerRebuyHandler : public BasicHandler {
public:
    virtual data::ENSessionState OnClientRequest(Session& session);
};

///////////////////////////////////////////////////////////
class GmOperationHandler : public BasicHandler {
public:
    virtual data::ENSessionState OnServerRequest(Session& session);
};

///////////////////////////////////////////////////////////
class ShowHoleCardsHandler : public BasicHandler {
public:
    virtual data::ENSessionState OnClientRequest(Session& session);
};

// vim:ts=4:sw=4:et:ft=cpp:
