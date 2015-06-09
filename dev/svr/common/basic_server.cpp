// File:        basic_server.cpp
// Description: ---
// Notes:       ---
// Author:      leoxiang <leoxiang727@qq.com>
// Revision:    2015-06-08 by leoxiang

#include <iostream>
#include "lsf/basic/type_cast.hpp"
#include "lsf/basic/macro.hpp"
#include "lsf/basic/string_ext.hpp"
#include "lsf/util/system.hpp"
#include "lsf/util/log.hpp"
#include "lsf/util/protobuf_log.hpp"
#include "../proto/msg_ss.pb.h"
#include "basic_server.h"

using namespace lsf::basic;
using namespace lsf::asio;
using namespace lsf::util;

void BasicServer::Run(int argc, char** argv)
{
    // bind log to terminal
    SingleLog::Instance()->BindOutput(new TermLogDriver());

    // init protobuf log
    ProtobufLog::Instance()->Init(); 
    
    // parse command
    if (!OnParseCommond(argc, argv)) return;

    // get config from confsvrd
    if (!OnGetConfig()) return;

    // set current path
    if (!OnSetCurrentPath(argv[0])) return;

    // bind log to local file
    if (!OnInitLocalLog()) return;

    // demonize
    if (!OnDeamonize()) return;

    // init proxy

    // init net log

    // call user init
    if (!OnRun()) return;

    // start async machine
    _io_service.Run();
}

bool BasicServer::OnParseCommond(int argc, char** argv)
{
    // check input
    if (argc < 4)
    {
        std::cerr << "usage: " << argv[0] << " [confsvrd_ip] [confsvrd_port] [server_id]" << std::endl;
        return false;
    }
    
    // parse content
    _confsvrd_addrss = tcp::SockAddr(argv[1], TypeCast<uint32_t>(argv[2]));
    _server_id = TypeCast<uint32_t>(argv[3]);

    return true;
}

bool BasicServer::OnGetConfig()
{
    // init a connection
    tcp::Socket socket;
    if (!socket.Connect(_confsvrd_addrss))
    {
        LSF_LOG_ERR("addr=%s, %s", _confsvrd_addrss.ToCharStr(), socket.ErrCharStr());
        socket.Close();
        return false;
    }

    // send request
    msg::SS message;
    std::string content;
    message.set_type(msg::SS_GET_CONFIG_REQ);
    message.mutable_get_config_req()->set_server_type(_server_type);
    message.mutable_get_config_req()->set_server_id(_server_id);
    if (!message.SerializeToString(&content))
    {
        LSF_LOG_ERR("%s", ProtobufLog::Instance()->ErrCharStr());
        socket.Close();
        return false;
    }
    if (!socket.SendAll(content))
    {
        LSF_LOG_ERR("size=%u, %s", content.size(), socket.ErrCharStr());
        socket.Close();
        return false;
    }

    // get response
    if (!socket.RecvAll(content))
    {
        LSF_LOG_ERR("%s", socket.ErrCharStr());
        socket.Close();
        return false;
    }

    // parse config
    if (!message.ParseFromString(content))
    {
        LSF_LOG_ERR("size=%u, %s", content.size(), ProtobufLog::Instance()->ErrCharStr());
        socket.Close();
        return false;
    }
    if (!message.get_config_rsp().result())
    {
        LSF_LOG_ERR("get config failed");
        socket.Close();
        return false;
    }
    _server_config.CopyFrom(message.get_config_rsp().config());

    socket.Close();
    return true;
}

bool BasicServer::OnSetCurrentPath(char const * command)
{
    std::string path = StringExt::GetDirName(System::GetAbsPath(command));
    if (!System::ChDir(path))
    {
        LSF_LOG_ERR("%s", System::ErrCharStr());
        return false;
    }

    return true;
}

bool BasicServer::OnInitLocalLog()
{
    if (!SingleLog::Instance()->BindOutput(new FileLogDriver(
                    _server_config.local_log_path(), FileLogDriver::SHIFT_DAY)))
    {
        LSF_LOG_ERR("%s", SingleLog::Instance()->ErrCharStr());
        return false;
    }
    return true;
}

bool BasicServer::OnDeamonize()
{
    System::Daemonize();
    return true;
}

bool BasicServer::OnInitProxy()
{
    return true;
}

bool BasicServer::OnInitNetLog()
{
    return true;
}

// vim:ts=4:sw=4:et:ft=cpp:
