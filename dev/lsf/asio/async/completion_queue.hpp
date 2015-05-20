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
    EN_COMPLETION_TYPE_TIMER  = 4
};

struct CompletionFunc
{
    ENCompletionType        type;
    std::function<bool()>   func;
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
    bool AddCompletionTask(int fd, ENCompletionType type, HandlerType func) {
        CompletionFunc & completion_func = (type == EN_COMPLETION_TYPE_WRITE ? _write_func[fd] : _read_func[fd]);
        completion_func.type = type;
        completion_func.func = std::function<bool()>(func);
        return true;
    }

    bool GetReadCompletionTask(int fd, CompletionFunc ** pfunc)
    {
        func_map_type::iterator iter = _read_func.find(fd);
        if (iter == _read_func.end()) return false;
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
    
};

} // end of namespace async
} // end of namespace asio
} // end of namespace lsf

// vim:ts=4:sw=4:et:ft=cpp:
