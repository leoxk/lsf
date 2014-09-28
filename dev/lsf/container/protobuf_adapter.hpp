// File:        protobuf_adapter.hpp
// Description: ---
// Notes:       ---
// Author:      leoxiang <xiangkun@ximigame.com>
// Revision:    2014-09-24 by leoxiang
//
// Copyright (C) 1998-2014 XiMi. All Rights Reserved.
// This is unpublished proprietary source code of XiMi Ltd. The copyright
// notice above does not evidence any actual or intended publication of such
// source code. UNAUTHORIZED DISTRIBUTION, ADAPTATION OR USE MAY BE SUBJECT
// TO CIVIL AND CRIMINAL PENALTIES.

#pragma once

namespace lsf {
namespace container {

template<typename PROTO_TYPE>
class ProtoBufAdapter
{
public:
    ProtoBufAdapter();
    ~ProtoBufAdapter();

private:
    uint8_t _is_serialized;
    uint8_t _buf[sizeof(PROTO_TYPE)];
}

} // end of namespace container
} // end of namespace lsf

// vim:ts=4:sw=4:et:ft=cpp:
