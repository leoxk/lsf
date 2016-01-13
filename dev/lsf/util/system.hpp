// File:        system.hpp
// Description: ---
// Notes:
// Author:      leoxiang <leoxiang727@qq.com>
// Revision:    2012-05-19 by leoxiang

#pragma once

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/resource.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <limits.h>
#include <dirent.h>
#include <ifaddrs.h>
#include <cstdio>
#include <cstdlib>
#include <string>
#include <utility>
#include <vector>
#include "lsf/basic/error.hpp"
#include "lsf/basic/scope_exit.hpp"
#include "lsf/asio/address.hpp"
#include "lsf/util/string_ext.hpp"

namespace lsf {
namespace util {

class System {
public:
    static const int DEF_MAX_NOFILE = 1024;
    static const int DEF_MAX_CORE_SIZE = 1 << 26;  // 64MB
    using handler_type = void(int);

public:
    ////////////////////////////////////////////////////////////
    // filesystem related
    static bool ChDir(std::string const& path) {
        if (::chdir(path.c_str()) < 0) return false;
        return true;
    }

    static std::string GetAbsPath(std::string const& path) {
        char buffer[PATH_MAX];
        return ::realpath(path.c_str(), buffer) == nullptr ? "" : buffer;
    }

    static std::string GetPwd() {
        char buffer[PATH_MAX];
        return ::getcwd(buffer, sizeof(buffer)) == nullptr ? "" : buffer;
    }

    static bool IsFile(std::string const& path) { return S_ISREG(GetFileMode(path)); }
    static bool IsDir(std::string const& path) { return S_ISDIR(GetFileMode(path)); }
    static bool IsSock(std::string const& path) { return S_ISSOCK(GetFileMode(path)); }

    static bool IsExist(std::string const& path) { return ::access(path.c_str(), F_OK) == 0; }
    static bool IsReadable(std::string const& path) { return ::access(path.c_str(), R_OK) == 0; }
    static bool IsWritable(std::string const& path) { return ::access(path.c_str(), W_OK) == 0; }
    static bool IsExecutable(std::string const& path) { return ::access(path.c_str(), X_OK) == 0; }

    static mode_t GetFileMode(std::string const& path) {
        struct stat tmp;
        if (::stat(path.c_str(), &tmp) < 0) return 0;
        return tmp.st_mode;
    }

    static bool MkDir(std::string const& path) {
        if (IsDir(path)) return true;

        std::string dirname = util::StringExt::GetDirName(path);

        if (IsExist(dirname) && !IsDir(dirname)) return false;

        // mk parent dir first
        if (!IsExist(dirname))
            if (!MkDir(dirname)) return false;

        // finally mk the dir
        if (::mkdir(path.c_str(), 0755) < 0) return false;

        return true;
    }

    static bool Rm(std::string const& path) { return RunShellCmd(std::string("rm -rf ") + path); }

    static bool Rename(std::string const& old_path, std::string const& new_path) {
        return std::rename(old_path.c_str(), new_path.c_str()) >= 0;
    }

    static bool RunShellCmd(std::string const& cmd) {
        if (cmd.empty()) return false;

        return ::system(cmd.c_str()) >= 0;
    }

    template<typename FuncType>
    static bool ForFilesWithinFold(std::string const& folder_path, FuncType&& func) {
        DIR* pdir = ::opendir(folder_path.c_str());
        struct dirent ent;
        struct dirent* pend = nullptr;
        while (::readdir_r(pdir, &ent, &pend) == 0) {
            if (pend == nullptr) return true; // end of dir
            if (::strcmp(pend->d_name, "..") == 0) continue; // ignore .
            if (::strcmp(pend->d_name, ".") == 0) continue; // ignore ..
            func(folder_path + '/' + pend->d_name);
        }
        return false;
    }

    ////////////////////////////////////////////////////////////
    // signal related
    static bool SetSignal(int sig_num, handler_type sig_handler) {
        struct sigaction sa;
        std::memset(&sa, 0, sizeof(sa));
        sa.sa_handler = sig_handler;
        ::sigemptyset(&sa.sa_mask);
        // restart interrupted system call
        sa.sa_flags = SA_RESTART;
        if (::sigaction(sig_num, &sa, nullptr) < 0) return false;
        return true;
    }

    ////////////////////////////////////////////////////////////
    // system related funcs
    static bool SetMaxNofile(int max_nofile = DEF_MAX_NOFILE) {
        rlimit rlim;
        rlim.rlim_cur = max_nofile;
        rlim.rlim_max = max_nofile;

        if (-1 == ::setrlimit(RLIMIT_NOFILE, &rlim)) return false;
        return true;
    }

    static bool SetMaxCore(int max_core = DEF_MAX_CORE_SIZE) {
        rlimit rlim;
        rlim.rlim_cur = max_core;
        rlim.rlim_max = max_core;

        if (-1 == ::setrlimit(RLIMIT_CORE, &rlim)) return false;
        return true;
    }

    static std::pair<int, int> GetMaxNofile() {
        rlimit rlim;
        ::getrlimit(RLIMIT_NOFILE, &rlim);
        return std::make_pair(rlim.rlim_cur, rlim.rlim_max);
    }

    static std::pair<int, int> GetMaxCore() {
        rlimit rlim;
        ::getrlimit(RLIMIT_CORE, &rlim);
        return std::make_pair(rlim.rlim_cur, rlim.rlim_max);
    }

    static void Daemonize() {
        // clear file creation mask
        ::umask(0);

        // become session leader to give up controlling tty
        if (::fork() != 0) std::exit(EXIT_SUCCESS);
        ::setsid();

        // redirect STDIN, STDOUT, STDERR
        int fd = ::open("/dev/null", O_RDWR);
        ::close(STDIN_FILENO);
        ::close(STDOUT_FILENO);
        ::close(STDERR_FILENO);
        ::dup2(fd, STDIN_FILENO);
        ::dup2(fd, STDOUT_FILENO);
        ::dup2(fd, STDERR_FILENO);
        ::close(fd);
    }

    static bool IsRoot() { return ::geteuid() == 0; }

    ////////////////////////////////////////////////////////////
    // network related
    static bool GetAllAddress(std::vector<lsf::asio::Address>& address_vec) {
        // get all ifs
        struct ifaddrs* all_ifa = nullptr;
        if (::getifaddrs(&all_ifa) < 0) return false;
        LSF_SCOPE_EXIT() { freeifaddrs(all_ifa); };

        // traverse all ifs
        for (auto* ifa = all_ifa; ifa != nullptr; ifa = ifa->ifa_next) {
            if (!ifa->ifa_addr) continue;

            // ipv4
            if (ifa->ifa_addr->sa_family == AF_INET) {
                address_vec.emplace_back(((struct sockaddr_in*)ifa->ifa_addr)->sin_addr);
            }
            // ipv6
            else if (ifa->ifa_addr->sa_family == AF_INET6) {
                address_vec.emplace_back(((struct sockaddr_in6*)ifa->ifa_addr)->sin6_addr);
            }
        }

        return true;
    }

    static lsf::asio::Address GetExternalAddress() {
        // get all
        std::vector<lsf::asio::Address> address_vec;
        if (!GetAllAddress(address_vec)) return lsf::asio::Address();

        // get first external
        for (auto const& address : address_vec) {
            if (address.IsV4() && !address.IsPrivate() && !address.IsLoopback()) return address;
        }

        return lsf::asio::Address();
    }

    static lsf::asio::Address GetInternalAddress() {
        // get all
        std::vector<lsf::asio::Address> address_vec;
        if (!GetAllAddress(address_vec)) return lsf::asio::Address();

        // get first internal
        for (auto const& address : address_vec) {
            if (address.IsV4() && address.IsPrivate()) return address;
        }

        return lsf::asio::Address();
    }
};

}  // end of namespace util
}  // end of namespace lsf

// vim:ts=4:sw=4:et:ft=cpp:
