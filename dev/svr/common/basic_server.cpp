// File:        basic_server.cpp
// Description: ---
// Notes:       ---
// Author:      leoxiang <leoxiang727@qq.com>
// Revision:    2015-06-08 by leoxiang

#include <iostream>
#include <functional>  
#include "svr/proto/msg_ss.pb.h"
#include "svr/common/basic_server.h"
#include "svr/common/common_func.h"
#include "svr/common/common_header.h"

using namespace lsf::basic;
using namespace lsf::asio;
using namespace lsf::util;

////////////////////////////////////////////////////////////
// utility funcs
conf::Service const * BasicServer::GetServiceConfig(conf::ENServiceType service_type)
{
    for (conf::Service const & iter : _server_config.services())
    {
        if (iter.service_type() == service_type) return &iter;
    }
    LSF_LOG_ERR("type=%u", service_type);
    return NULL;
}

////////////////////////////////////////////////////////////
// init logic
void BasicServer::Run(int argc, char** argv)
{
    // bind log to terminal
    SingleLog::Instance()->BindOutput(new TermLogDriver());

    // init protobuf log
    ProtobufLog::Instance()->Init(); 
    
    // parse command
    if (!OnParseCommond(argc, argv)) return;

    // get config from confsvrd
    if (!OnInitDeployConfig()) return;

    // set current path
    if (!OnSetCurrentPath(argv[0])) return;

    // bind log to local file
    if (!OnInitLocalLog()) return;

    // demonize
    if (!OnDeamonize()) return;

    // main logic
    if (!OnRun()) return;

    // start async machine
    IOService::Instance()->Run();
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
    _confsvrd_addrss = tcp::SockAddr(ip::Address(argv[1]), TypeCast<uint32_t>(argv[2]));
    _server_id = TypeCast<uint32_t>(argv[3]);
    _server_name = StringExt::GetBaseName(argv[0]);

    return true;
}

bool BasicServer::OnInitDeployConfig()
{
    // init a connection
    tcp::Socket socket = tcp::Socket::CreateSocket();
    if (!socket.Connect(_confsvrd_addrss))
    {
        LSF_LOG_ERR("addr=%s, %s", _confsvrd_addrss.ToCharStr(), socket.ErrCharStr());
        socket.Close();
        return false;
    }

    // construct message
    msg::SS message;
    std::string content;
    message.set_type(msg::SS_GET_CONFIG_REQ);
    message.mutable_get_config_req()->set_server_type(_server_type);
    message.mutable_get_config_req()->set_server_id(_server_id);

    // send and recv
    if (!CommonFunc::SendAndRecv(socket, message))
    {
        socket.Close();
        return false;
    }

    // check ret
    if (!message.get_config_rsp().result())
    {
        LSF_LOG_ERR("get config failed");
        socket.Close();
        return false;
    }
    _server_config.CopyFrom(message.get_config_rsp().config());

    // check server type and id
    if (_server_type != _server_config.server_type() ||
        _server_id != _server_config.server_id())
    {
        LSF_LOG_ERR("input=%u %u, config=%u %u", _server_type, _server_id, 
                _server_config.server_type(), _server_config.server_id());
        return false;
    }

    LSF_LOG_INFO("get config from confsvrd successs");
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

    LSF_LOG_INFO("set current path to %s", path.c_str());
    return true;
}

bool BasicServer::OnInitLocalLog()
{
    std::string local_log_path = 
        "../log/" + _server_name + "/" + _server_name + "." + TypeCast<std::string>(_server_id);

    if (!SingleLog::Instance()->BindOutput(new FileLogDriver(local_log_path, FileLogDriver::SHIFT_DAY)))
    {
        LSF_LOG_ERR("%s", SingleLog::Instance()->ErrCharStr());
        return false;
    }

    LSF_LOG_INFO("init local log at %s", local_log_path.c_str());
    return true;
}

bool BasicServer::OnDeamonize()
{
    System::Daemonize();
    return true;
}

// vim:ts=4:sw=4:et:ft=cpp:
