// File:        completion_queue.hpp
// Description: ---
// Notes:       ---
// Author:      leoxiang <leoxiang@tencent.com>
// Revision:    2012-06-12 by leoxiang

#pragma once

#include "lsf/util/noncopyable.hpp"

namespace lsf {
namespace asio {

typedef void (ReadHandler *)(size_t read_size);
typedef void (WriteHandler *)(size_t write_size);
typedef void (TimerHandler *)();

class CompletionQueue : public NonCopyable
{
public:
    const static size_t DEL_QUEUE_SIZE = 65536;
    

private:
    
};

} // end of namespace asio
} // end of namespace lsf

// vim:ts=4:sw=4:et:ft=cpp:
