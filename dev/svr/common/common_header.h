// File:        common_header.h
// Description: ---
// Notes:       ---
// Author:      leoxiang <leoxiang727@qq.com>
// Revision:    2015-06-10 by leoxiang

#pragma once

#include "google/protobuf/io/zero_copy_stream_impl.h"
#include "google/protobuf/text_format.h"
#include "lsf/asio/tcp.hpp"
#include "lsf/asio/io_service.hpp"
#include "lsf/basic/noncopyable.hpp"
#include "lsf/basic/type_cast.hpp"
#include "lsf/basic/macro.hpp"
#include "lsf/basic/error.hpp"
#include "lsf/basic/string_ext.hpp"
#include "lsf/util/system.hpp"
#include "lsf/util/log.hpp"
#include "lsf/util/protobuf_log.hpp"
#include "lsf/basic/buffer.hpp"

namespace common {

static const uint8_t MAGIC_STRING[] = { 0xf8, 0x8f };

} // end of namespace common


// vim:ts=4:sw=4:et:ft=cpp:
