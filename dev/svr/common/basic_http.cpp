// File:        basic_http.cpp
// Description: This imply file for http service with mongoose
// Notes:       ---
// Author:      Haust <wyy123_2008@qq.com>
// Revision:    2015-11-19 by Haust

#include "basic_http.h"
#include "lsf/util/string_ext.hpp"
#include "svr/common/common_header.h"

#include <sstream>

////////////////////////////////////////////////////////////////////////
//Base64
std::string Base64Decode(std::string const& instr){
    auto length = instr.length()/4*3;
    auto* buffer = new char[length+1];
    mg_base64_decode((unsigned char *)instr.data(), instr.length(), buffer);
    std::string outstr(buffer, length);
    delete []buffer;
    return outstr;
}

std::string Base64Encode(std::string const& instr){
    auto length = (instr.length()+2)/3*4;
    auto* buffer = new char[length+1];
    mg_base64_encode((unsigned char *)instr.data(), instr.length(), buffer);
    std::string outstr(buffer, length);
    delete []buffer;
    return outstr;
}

////////////////////////////////////////////////////////////////////
BasicHttp::handler_map BasicHttp::_handlers;
BasicHttp::connection_map BasicHttp::_connections;

void BasicHttp::Init(uint32_t port){
    memset(_port, 0, sizeof(_port));
    snprintf(_port, sizeof(_port), "%u", port);
}

bool BasicHttp::Start(){
    mg_mgr_init(&_mgr, NULL);
    auto nc = mg_bind(&_mgr, _port, EvHandler);

    if(nullptr == nc)
        return false;

    mg_set_protocol_http_websocket(nc);
    return true;
}

bool BasicHttp::Close(){
    mg_mgr_free(&_mgr);
    return true;
}

bool BasicHttp::RegisterHandler(std::string uri, handler f){
    auto it = _handlers.find(uri);
    if(_handlers.end() != it)
        return false;

    return _handlers.emplace(uri, f).second;
}

void BasicHttp::UnRegisterHandler(std::string uri){
    auto it = _handlers.find(uri);
    if(_handlers.end() != it)
        _handlers.erase(it);
}

void BasicHttp::Loop(int milli){
    mg_mgr_poll(&_mgr, milli);
}

void BasicHttp::EvHandler(struct mg_connection* nc, int ev, void* ev_data){
   switch(ev){
        case MG_EV_HTTP_REQUEST:
            HandleRequst(nc, ev, ev_data);
            break;
       default:
           break;
   }
}

void BasicHttp::HandleRequst(struct mg_connection *nc, int ev, void* ev_data){
    http_message* hm = (http_message*)ev_data;
    std::string uri(hm->uri.p, hm->uri.len);
    std::string query(hm->query_string.p, hm->query_string.len);
    std::string body(hm->body.p, hm->body.len);

    auto it = _handlers.find(uri);
    if(_handlers.end() == it){
        std::ostringstream oss;
        oss << "HTTP/1.1 404 Not Found\r\n\r\n404 Not Found\r\n";
        mg_send(nc, oss.str().c_str(), oss.str().length());
        nc->flags |= MG_F_SEND_AND_CLOSE;
        LSF_LOG_INF("uri:%s, query:%s,reply:%s", uri.c_str(), query.c_str(), oss.str().c_str());
        return;
    }

    _connections.emplace(uri, nc);
    it->second(query, body);
    LSF_LOG_INF("GetRequest: uri:%s, query:%s, body:%s", uri.c_str(), query.c_str(), body.c_str());
}

void BasicHttp::SendReply(std::string uri, std::string reply){
    auto range = _connections.equal_range(uri);
    if(range.first == range.second)
        return;

    std::string reply_all("HTTP/1.1 200 OK\r\nConnection: close\r\nContent-Type: text/html\r\nContent-Length:");
    reply_all.append(lsf::util::StringExt::Format("%u", (uint32_t)reply.length()))
             .append("\r\n\r\n").append(reply).append("\r\n");

    auto it = range.first;
    mg_send(it->second, reply_all.c_str(), reply_all.length());
    it->second->flags |= MG_F_SEND_AND_CLOSE;
    _connections.erase(it);
    LSF_LOG_INF("uri:%s, reply:%s", uri.c_str(), reply_all.c_str());
}

void BasicHttp::SendError(std::string uri, int errcode, std::string reply){
    auto range = _connections.equal_range(uri);
    if(range.first == range.second)
        return;

    std::ostringstream oss;
    oss << "HTTP/1.1 " << errcode << " " << reply.c_str() << "\r\n\r\n" << reply.c_str() << "\r\n";

    auto it = range.first;
    mg_send(it->second, oss.str().c_str(), oss.str().length());

    it->second->flags |= MG_F_SEND_AND_CLOSE;
    _connections.erase(it);
    LSF_LOG_INF("uri:%s, reply:%s", uri.c_str(), oss.str().c_str());
}

void BasicHttp::SendFile(std::string uri, std::string content){
    auto range = _connections.equal_range(uri);
    if(range.first == range.second)
        return;

    auto it = range.first;
    auto reply = std::string("HTTP/1.1 200 OK\r\nConnection: close\r\nContent-Type: zip\r\nContent-Length: ")
        .append(lsf::util::StringExt::Format("%u", content.length())).append("\r\n\r\n").append(content);

    mg_send(it->second, reply.c_str(), reply.length());
    it->second->flags |= MG_F_SEND_AND_CLOSE;
    _connections.erase(it);
    LSF_LOG_INF("uri:%s, reply:%s", uri.c_str(), reply.c_str()); 
}

#include "svr/3rd/mongoose.c"
