// File:        compile_unit.cpp
// Description: ---
// Notes:       ---
// Author:      leoxiang <leoxiang@tencent.com>
// Revision:    2012-06-01 by leoxiang

#include "lsf/util/config.hpp"
#include "lsf/util/random.hpp"
//#include "lsf/util/log.hpp"
//#include "lsf/util/system.hpp"
//#include "lsf/util/shared_mem.hpp"

//#include "lsf/basic/string_ext.hpp"
//#include "lsf/basic/type_cast.hpp"
//#include "lsf/basic/singleton.hpp"
#include "lsf/basic/unit_test.hpp"

//#include "lsf/encrypt/base64.hpp"
//#include "lsf/encrypt/md5.hpp"

//#include "lsf/container/queue.hpp"

void UnitOne() 
{
    lsf::util::SingleConfig::Instance()->ParseFromString("test == test");

    lsf::util::SingleRandom::Instance()->GetRand(65535);
}

// vim:ts=4:sw=4:et:ft=cpp:
