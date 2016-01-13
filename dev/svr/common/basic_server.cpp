// File:        basic_server.cpp
// Description: ---
// Notes:       ---
// Author:      leoxiang <leoxiang727@qq.com>
// Revision:    2015-06-08 by leoxiang

#include <iostream>
#include <functional>
#include "svr/common/common_proto.h"
#include "svr/common/common_func.h"
#include "svr/common/common_header.h"
#include "svr/common/common_service.h"
#include "svr/common/basic_server.h"
#include "svr/common/config_manager.h"

using namespace lsf::basic;
using namespace lsf::asio;
using namespace lsf::util;

////////////////////////////////////////////////////////////
// utility funcs
conf::ConnectService const* BasicServer::GetConnectServiceConfig(conf::ENServiceType service_type) {
    for (conf::ConnectService const& iter : ConfigManager::Instance()->ServerConfig().connect_services()) {
        if (iter.service_type() == service_type) return &iter;
    }
    LSF_LOG_ERR_STACK("get config failed, type=%s", LSF_ETS(service_type));
    return nullptr;
}

conf::AcceptService const* BasicServer::GetAcceptServiceConfig(conf::ENServiceType service_type) {
    for (conf::AcceptService const& iter : ConfigManager::Instance()->ServerConfig().accept_services()) {
        if (iter.service_type() == service_type) return &iter;
    }
    LSF_LOG_ERR_STACK("get config failed, type=%s", LSF_ETS(service_type));
    return nullptr;
}

////////////////////////////////////////////////////////////
// init logic
void BasicServer::Run(int argc, char** argv) {
    // save variable
    _argc = argc;
    _argv = argv;

    // init protobuf log
    Protobuf::Instance()->Init();

    // parse command
    if (!OnParseCommond()) return;

    // set current path
    if (!OnInitCurrentPath()) return;

    // get config from confsvrd
    if (!OnInitConfigService()) return;

    // demonize
    if (!OnDeamonize()) return;

    // bind log to local file
    if (!OnInitLocalLog()) return;

    // init signal
    if (!OnInitSignal()) return;

    // main logic
    if (!OnRun()) return;

    // start async machine
    IOService::Instance()->Run();
}

bool BasicServer::OnParseCommond() {
    // check input
    if (_argc < 3) {
        std::cerr << "usage: " << _argv[0] << " [confsvrd_ip] [confsvrd_port] [server_id]" << std::endl;
        return false;
    }

    // parse content
    _confsvrd_addrss = std::string(_argv[1]) + "|" + _argv[2];
    _server_name = StringExt::GetBaseName(_argv[0]);
    _server_id = _argc == 3 ? 0 : TypeCast<uint32_t>(_argv[3]);

    return true;
}

bool BasicServer::OnInitConfigService() {
   // init connect config service
    ConnectConfigService::Instance()->SetConfigServiceAddress(_confsvrd_addrss);
    if (!ConnectConfigService::Instance()->Run(this)) return false;

    return true;
}

bool BasicServer::OnInitCurrentPath() {
    std::string path = System::GetAbsPath(StringExt::GetDirName(_argv[0]) + "/..");
    if (!System::ChDir(path)) {
        LSF_LOG_ERR_STACK("set current path failed, %s, %s", path.c_str(), LSF_SES());
        return false;
    }

    LSF_LOG_INF("set current path to %s", path.c_str());
    return true;
}

bool BasicServer::OnInitLocalLog() {
    std::string local_log_path =
        "log/" + _server_name + "/" + _server_name + "." + TypeCast<std::string>(_server_id);

    if (!SingleLog::Instance()->BindFile(local_log_path, LogFileBuf::SHIFT_DAY)) {
        LSF_LOG_ERR("bind log file fail, %s", local_log_path.c_str());
        return false;
    }

    LSF_LOG_INF("init local log at %s", local_log_path.c_str());
    return true;
}

bool BasicServer::OnDeamonize() {
    System::Daemonize();
    return true;
}

////////////////////////////////////////////////////////////
// signal helper
namespace detail {

static BasicServer* pserver = nullptr;

void SignalHandler(int sig) { pserver->OnSignalHandle(sig); }

}  // end of namespace detail

bool BasicServer::OnInitSignal() {
    // set pserver
    ::detail::pserver = this;

    System::SetSignal(SIGINT,  ::detail::SignalHandler);
    System::SetSignal(SIGHUP,  ::detail::SignalHandler);
    System::SetSignal(SIGQUIT, ::detail::SignalHandler);
    System::SetSignal(SIGPIPE, ::detail::SignalHandler);
    System::SetSignal(SIGTTOU, ::detail::SignalHandler);
    System::SetSignal(SIGTTIN, ::detail::SignalHandler);
    System::SetSignal(SIGTERM, ::detail::SignalHandler);
    System::SetSignal(SIGABRT, ::detail::SignalHandler);
    System::SetSignal(SIGSEGV, ::detail::SignalHandler);
    System::SetSignal(SIGUSR1, ::detail::SignalHandler);
    System::SetSignal(SIGUSR2, ::detail::SignalHandler);

    return true;
}

void BasicServer::OnSignalHandle(int sig) {
    if (sig == SIGSEGV) {
        OnExit();
        System::SetSignal(SIGSEGV, SIG_DFL);
    }
    if (sig == SIGUSR1) {
        OnExit();
        IOService::Instance()->SetExit();
    }
    if (sig == SIGUSR2) {
    }
}

// vim:ts=4:sw=4:et:ft=cpp:
