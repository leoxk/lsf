// File:        test_multi_compile_unit.cpp
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

#include "compile_unit.h"

LSF_TEST_CASE(test_singleconfig)
{
    LSF_ASSERT(lsf::util::SingleConfig::Instance()->Size() == 0);
    LSF_ASSERT(lsf::util::SingleRandom::Instance()->GetUseCount() == 0);

    UnitOne(); 
    LSF_ASSERT(lsf::util::SingleConfig::Instance()->Size() == 1);
    LSF_ASSERT(lsf::util::SingleConfig::Reference().Get("test") == "test");

    LSF_ASSERT(lsf::util::SingleRandom::Instance()->GetUseCount() == 1);
}

int main(int argc, char** argv)
{
	LSF_TEST_ALL();
}

// vim:ts=4:sw=4:et:ft=cpp:
