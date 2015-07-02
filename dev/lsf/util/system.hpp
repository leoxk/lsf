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
#include <cstdio>
#include <cstdlib>
#include <string>
#include <utility>
#include "lsf/basic/error.hpp"
#include "lsf/util/string_ext.hpp"

namespace lsf {
namespace util {

class System : public lsf::basic::ErrorStatic {
public:
    static const int DEF_MAX_NOFILE = 1024;
    static const int DEF_MAX_CORE_SIZE = 1 << 26;  // 64MB

    typedef void (*handler_type)(int);

public:
    // static funcs
    static bool ChDir(std::string const& path) {
        if (ErrWrap(::chdir(path.c_str())) < 0) return false;
        return true;
    }

    static std::string GetAbsPath(std::string const& path) {
        static char buffer[PATH_MAX];
        return ::realpath(path.c_str(), buffer) == nullptr ? "" : buffer;
    }

    static std::string GetPwd() {
        static char buffer[PATH_MAX];
        return ::getcwd(buffer, sizeof(buffer)) == nullptr ? "" : buffer;
    }

    static bool IsFile(std::string const& path) { return S_ISREG(GetFileMode(path)); }
    static bool IsDir(std::string const& path) { return S_ISDIR(GetFileMode(path)); }
    static bool IsSock(std::string const& path) { return S_ISSOCK(GetFileMode(path)); }

    static bool IsExist(std::string const& path) { return ::access(path.c_str(), F_OK) == 0; }
    static bool IsReadable(std::string const& path) { return ::access(path.c_str(), R_OK) == 0; }
    static bool IsWritable(std::string const& path) { return ::access(path.c_str(), W_OK) == 0; }
    static bool IsExecutable(std::string const& path) { return ::access(path.c_str(), X_OK) == 0; }

    static bool MkDir(std::string const& path) {
        if (IsDir(path)) return true;

        std::string dirname = util::StringExt::GetDirName(path);

        if (IsExist(dirname) && !IsDir(dirname)) return false;

        // mk parent dir first
        if (!IsExist(dirname))
            if (!MkDir(dirname)) return false;

        // finally mk the dir
        if (ErrWrap(::mkdir(path.c_str(), 0755)) < 0) return false;

        return true;
    }

    static bool Rm(std::string const& path) { return RunShellCmd(std::string("rm -rf ") + path); }

    static bool Rename(std::string const& old_path, std::string const& new_path) {
        return ErrWrap(std::rename(old_path.c_str(), new_path.c_str())) >= 0;
    }

    static bool RunShellCmd(std::string const& cmd) {
        if (cmd.empty()) return false;

        return ErrWrap(::system(cmd.c_str())) >= 0;
    }

    static bool IsRoot() { return ::geteuid() == 0; }

    // signal
    static bool SetSignal(int sig_num, handler_type sig_handler) {
        struct sigaction sa;
        std::memset(&sa, 0, sizeof(sa));
        sa.sa_handler = sig_handler;
        ::sigemptyset(&sa.sa_mask);
        // restart interrupted system call
        sa.sa_flags = SA_RESTART;
        if (ErrWrap(::sigaction(sig_num, &sa, nullptr)) < 0) return false;
        return true;
    }

    // rlimit related funcs
    static bool SetMaxNofile(int max_nofile = DEF_MAX_NOFILE) {
        rlimit rlim;
        rlim.rlim_cur = max_nofile;
        rlim.rlim_max = max_nofile;

        if (-1 == ErrWrap(::setrlimit(RLIMIT_NOFILE, &rlim))) return false;
        return true;
    }

    static bool SetMaxCore(int max_core = DEF_MAX_CORE_SIZE) {
        rlimit rlim;
        rlim.rlim_cur = max_core;
        rlim.rlim_max = max_core;

        if (-1 == ErrWrap(::setrlimit(RLIMIT_CORE, &rlim))) return false;
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

    static mode_t GetFileMode(std::string const& path) {
        struct stat tmp;
        if (::stat(path.c_str(), &tmp) < 0) return 0;
        return tmp.st_mode;
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
};

}  // end of namespace util
}  // end of namespace lsf

// vim:ts=4:sw=4:et:ft=cpp:
