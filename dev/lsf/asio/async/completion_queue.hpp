// File:        completion_queue.hpp
// Description: ---
// Notes:       ---
// Author:      leoxiang <leoxiang727@qq.com>
// Revision:    2012-06-12 by leoxiang

#pragma once

#include <functional>
#include <map>
#include "lsf/basic/noncopyable.hpp"
#include "lsf/basic/error.hpp"
#include "lsf/basic/buffer.hpp"

namespace lsf {
namespace asio {
namespace async {

////////////////////////////////////////////////////////////
// Function Closure
////////////////////////////////////////////////////////////
enum ENCompletionType {
    EN_COMPLETION_TYPE_ACCEPT = 1,
    EN_COMPLETION_TYPE_READ   = 2,
    EN_COMPLETION_TYPE_WRITE  = 3,
    EN_COMPLETION_TYPE_TIMER  = 4,
    EN_COMPLETION_TYPE_RDHUP  = 5
};

struct AsyncInfo
{
public:
    AsyncInfo() : fd(0), accept_fd(0) { buffer.clear(); }
    void Clear() { fd = 0; accept_fd = 0; buffer.clear(); }

public:
    int         fd;
    int         accept_fd;
    std::string buffer;
};

struct CompletionFunc
{
    typedef std::function<bool(AsyncInfo &)>  func_type;
    ENCompletionType    type;
    func_type           func;
};

////////////////////////////////////////////////////////////
// Completion Queue
////////////////////////////////////////////////////////////
class CompletionQueue : 
    public basic::NonCopyable,
    public basic::Error
{
public:
    typedef std::map<int,CompletionFunc>    func_map_type;

    const static size_t DEL_QUEUE_SIZE = 65536;
    
public:
    template<typename HandlerType>
    bool AddCompletionTask(int fd, ENCompletionType type, HandlerType func)
    {
        CompletionFunc * pfunc = NULL;
        switch (type)
        {
            case EN_COMPLETION_TYPE_ACCEPT:
            case EN_COMPLETION_TYPE_READ:
            case EN_COMPLETION_TYPE_TIMER:
                pfunc = &_read_func[fd];
                break;
                
            case EN_COMPLETION_TYPE_WRITE:
                pfunc = &_write_func[fd];
                break;

            case EN_COMPLETION_TYPE_RDHUP:
                pfunc = &_rdhup_func[fd];
                break;

            default:
                break;
        }
        if (pfunc == NULL) return false;

        pfunc->type = type;
        pfunc->func = CompletionFunc::func_type(func);
        return true;
    }

    void CancelCompletionTask(int fd)
    {
        _read_func.erase(fd);
        _write_func.erase(fd);
    }

    bool GetReadCompletionTask(int fd, CompletionFunc ** pfunc)
    {
        func_map_type::iterator iter = _read_func.find(fd);
        if (iter == _read_func.end()) return false;
        *pfunc = &iter->second;
        return true;
    }

    bool GetRdHupCompletionTask(int fd, CompletionFunc ** pfunc)
    {
        func_map_type::iterator iter = _rdhup_func.find(fd);
        if (iter == _rdhup_func.end()) return false;
        *pfunc = &iter->second;
        return true;
    }

    bool GetWriteCompletionTask(int fd, CompletionFunc ** pfunc)
    {
        func_map_type::iterator iter = _write_func.find(fd);
        if (iter == _write_func.end()) return false;
        *pfunc = &iter->second;
        return true;

    }

private:
    func_map_type   _read_func;
    func_map_type   _write_func;
    func_map_type   _rdhup_func;;
};

} // end of namespace async
} // end of namespace asio
} // end of namespace lsf

// vim:ts=4:sw=4:et:ft=cpp:
