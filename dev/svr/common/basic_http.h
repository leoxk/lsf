// File:        basic_http.h
// Description: ---
// Notes:       ---
// Author:      Haust <wyy123_2008@qq.com>
// Revision:    2015-11-19 by Haust

#pragma once

#include "svr/3rd/mongoose.h"
#include <map>
#include <string>
#include <functional>

////////////////////////////////////////////////////////////////
//Base64
std::string Base64Decode(std::string const& instr);
std::string Base64Encode(std::string const& instr);

//////////////////////////////////////////////////////////////////
class BasicHttp {
public:
	using handler = std::function<void(std::string, std::string)>;

public:
    virtual ~BasicHttp(){};

    //typedef void (*handler)(std::string query, std::string body);


    void Init(uint32_t port);
    bool Start();
    bool Close();
    bool RegisterHandler(std::string uri, handler f);
    void UnRegisterHandler(std::string uri);
    void Loop(int milli);

    void SendReply(std::string uri, std::string reply);
    void SendError(std::string uri, int errcode, std::string reply);

    void SendFile(std::string uri, std::string content);

protected:
    using handler_map = std::map<std::string, handler>;
    using connection_map = std::multimap<std::string, mg_connection*>;

private:
    static void EvHandler(struct mg_connection* nc, int ev, void* ev_data);
    static void HandleRequst(struct mg_connection* nc, int ev, void *ev_data);

public:
    static handler_map _handlers;
    static connection_map _connections;

    char _port[11];
    struct mg_mgr _mgr;

};
