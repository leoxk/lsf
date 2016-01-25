// File:        common_header.h
// Description: ---
// Notes:       ---
// Author:      leoxiang <leoxiang727@qq.com>
// Revision:    2015-06-10 by leoxiang

/*!
@mainpage

These pages contain the API documentation of LSF, which is short for light-weighted server framework.

@copyright The code is licensed under the [MIT
           License](http://opensource.org/licenses/MIT):
           <br>
           Permission is hereby granted, free of charge, to any person
           obtaining a copy of this software and associated documentation files
           (the "Software"), to deal in the Software without restriction,
           including without limitation the rights to use, copy, modify, merge,
           publish, distribute, sublicense, and/or sell copies of the Software,
           and to permit persons to whom the Software is furnished to do so,
           subject to the following conditions:
           <br>
           The above copyright notice and this permission notice shall be
           included in all copies or substantial portions of the Software.
           <br>
           THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
           EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
           MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
           NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
           BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
           ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
           CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
           SOFTWARE.
*/

#pragma once
#include <google/protobuf/io/zero_copy_stream_impl.h>
#include <google/protobuf/text_format.h>
#include <google/protobuf/message_lite.h>
#include "lsf/asio/async.hpp"
#include "lsf/asio/curl.hpp"
#include "lsf/basic/noncopyable.hpp"
#include "lsf/basic/macro.hpp"
#include "lsf/basic/error.hpp"
#include "lsf/basic/type_cast.hpp"
#include "lsf/basic/scope_exit.hpp"
#include "lsf/basic/unit_test.hpp"
#include "lsf/util/string_ext.hpp"
#include "lsf/util/system.hpp"
#include "lsf/util/log.hpp"
#include "lsf/util/protobuf.hpp"
#include "lsf/util/backtrace.hpp"
#include "lsf/util/random.hpp"
#include "lsf/util/serialize.hpp"

// vim:ts=4:sw=4:et:ft=cpp:
