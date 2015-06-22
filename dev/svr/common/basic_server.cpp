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
#include "svr/common/connect_config_service.h"

using namespace lsf::basic;
using namespace lsf::asio;
using namespace lsf::util;

////////////////////////////////////////////////////////////
// utility funcs
conf::ConnectService const* BasicServer::GetConnectServiceConfig(conf::ENServiceType service_type) {
    for (conf::ConnectService const& iter : _server_config.connect_service()) {
        if (iter.service_type() == service_type) return &iter;
    }
    LSF_LOG_ERR("get config failed, type=%u", service_type);
    return nullptr;
}

conf::AcceptService const* BasicServer::GetAcceptServiceConfig(conf::ENServiceType service_type) {
    for (conf::AcceptService const& iter : _server_config.accept_service()) {
        if (iter.service_type() == service_type) return &iter;
    }
    LSF_LOG_ERR("get config failed, type=%u", service_type);
    return nullptr;
}

////////////////////////////////////////////////////////////
// init logic
void BasicServer::Run(int argc, char** argv) {
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

    // init signal
    if (!OnInitSignal()) return;

    // set callback func
    IOService::Instance()->SetTickFunc(std::bind(&BasicServer::OnTick, this));
    IOService::Instance()->SetExitFunc(std::bind(&BasicServer::OnExit, this));

    // main logic
    if (!OnRun()) return;

    // start async machine
    IOService::Instance()->Run();
}

bool BasicServer::OnParseCommond(int argc, char** argv) {
    // check input
    if (argc < 4) {
        std::cerr << "usage: " << argv[0] << " [confsvrd_ip] [confsvrd_port] [server_id]" << std::endl;
        return false;
    }

    // parse content
    _confsvrd_addrss = std::string(argv[1]) + "|" + argv[2];
    _server_id = TypeCast<uint32_t>(argv[3]);
    _server_name = StringExt::GetBaseName(argv[0]);

    return true;
}

bool BasicServer::OnInitDeployConfig() {
    // init connect config service
    ConnectConfigService::Instance()->SetConfigServerAddress(_confsvrd_addrss);
    ConnectConfigService::Instance()->Run(this);

    // check server type and id
    if (_server_type != _server_config.server_type() || _server_id != _server_config.server_id()) {
        LSF_LOG_ERR("server not match, input=%u %u, config=%u %u", _server_type, _server_id,
                    _server_config.server_type(), _server_config.server_id());
        return false;
    }

    LSF_LOG_INFO("get config from confsvrd successs");

    return true;
}

bool BasicServer::OnSetCurrentPath(char const* command) {
    std::string path = StringExt::GetDirName(System::GetAbsPath(command));
    if (!System::ChDir(path)) {
        LSF_LOG_ERR("set current path failed, %s", System::ErrCharStr());
        return false;
    }

    LSF_LOG_INFO("set current path to %s", path.c_str());
    return true;
}

bool BasicServer::OnInitLocalLog() {
    std::string local_log_path =
        "../log/" + _server_name + "/" + _server_name + "." + TypeCast<std::string>(_server_id);

    if (!SingleLog::Instance()->BindOutput(new FileLogDriver(local_log_path, FileLogDriver::SHIFT_DAY))) {
        LSF_LOG_ERR("init local log failed, %s", SingleLog::Instance()->ErrCharStr());
        return false;
    }

    LSF_LOG_INFO("init local log at %s", local_log_path.c_str());
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

    System::SetSignal(SIGINT, ::detail::SignalHandler);
    System::SetSignal(SIGHUP, ::detail::SignalHandler);
    System::SetSignal(SIGQUIT, ::detail::SignalHandler);
    System::SetSignal(SIGPIPE, ::detail::SignalHandler);
    System::SetSignal(SIGTTOU, ::detail::SignalHandler);
    System::SetSignal(SIGTTIN, ::detail::SignalHandler);
    System::SetSignal(SIGTERM, ::detail::SignalHandler);
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
        IOService::Instance()->SetExit();
    }
    if (sig == SIGUSR2) {
    }
}

// vim:ts=4:sw=4:et:ft=cpp:
