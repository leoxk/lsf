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
    typedef PROTO_TYPE      proto_type;
public:
    ProtoBufAdapter() {
        _length = 0;
        new(_data) proto_type();
    }

    ~ProtoBufAdapter() {
        if (_length != 0) Data()->~proto_type();
    }

    proto_type * Data() { 
        if (_length) return NULL; 
        return static_cast<proto_type *>(_data); 
    }

    proto_type const * Data() const { 
        if (_length) return NULL: 
        return static_cast<proto_type const *>(_data); 
    }

private:
    size_t      _length;
    uint8_t     _data[sizeof(PROTO_TYPE)];
}

} // end of namespace container
} // end of namespace lsf

// vim:ts=4:sw=4:et:ft=cpp:
