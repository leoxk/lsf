// File:        test_system.cpp
// Description: ---
// Notes:       ---
// Author:      leoxiang <leoxiang727@qq.com>
// Revision:    2012-05-19 by leoxiang

#include "lsf/basic/unit_test.hpp"
#include "lsf/util/string_ext.hpp"
#include "lsf/util/system.hpp"

using namespace std;
using namespace lsf::util;
using namespace lsf::basic;
using namespace lsf::asio;

LSF_TEST_CASE(file_related) {
    LSF_ASSERT(System::IsDir("/"));
    LSF_ASSERT(System::IsDir("/bin"));

    LSF_ASSERT(System::IsFile("/bin/ls"));
    LSF_ASSERT(System::IsFile("/bin/cp"));

    LSF_ASSERT(System::IsReadable("/bin"));
    LSF_ASSERT(System::IsReadable("/bin/ls"));

    LSF_ASSERT(System::IsWritable(System::GetPwd()));

    LSF_ASSERT(StringExt::GetDirName(System::GetAbsPath("./test")) == System::GetPwd());

    LSF_ASSERT(System::IsExecutable("/bin/ls"));
    LSF_ASSERT(System::IsExecutable("/bin/cp"));
}

LSF_TEST_CASE(mkdir_and_rm) {
    if (System::IsExist("./test_lsf_system_lib")) System::Rm("./test_lsf_system_lib");

    LSF_ASSERT(System::MkDir("./test_lsf_system_lib/1/2/3"));
    LSF_ASSERT(System::IsDir("./test_lsf_system_lib/1/2/3"));
    LSF_ASSERT(System::IsDir("./test_lsf_system_lib/1/2"));
    LSF_ASSERT(System::IsDir("./test_lsf_system_lib/1"));
    LSF_ASSERT(System::IsDir("./test_lsf_system_lib/"));

    LSF_ASSERT(System::Rename("./test_lsf_system_lib", "./test_lsf_system_lib_rename"));
    LSF_ASSERT(System::Rm("./test_lsf_system_lib_rename"));
    LSF_ASSERT(!System::IsDir("./test_lsf_system_lib_rename"));
}

LSF_TEST_CASE(rlimit_funcs) {
    if (System::IsRoot()) {
        // default setting
        LSF_ASSERT(System::SetMaxNofile());
        LSF_ASSERT(System::GetMaxNofile().first == System::DEF_MAX_NOFILE);

        LSF_ASSERT(System::SetMaxCore());
        LSF_ASSERT(System::GetMaxCore().first == System::DEF_MAX_CORE_SIZE);

        // user setting
        LSF_ASSERT(System::SetMaxNofile(100000));
        LSF_ASSERT(System::GetMaxNofile().first == 100000);

        LSF_ASSERT(System::SetMaxCore(1000000));
        LSF_ASSERT(System::GetMaxCore().first == 1000000);
    } else {
        // with out root privilege, so set to hard max
        LSF_ASSERT(System::SetMaxNofile(System::GetMaxNofile().second));

        LSF_ASSERT(System::SetMaxCore(System::GetMaxCore().second));
    }
}

LSF_TEST_CASE(test_network) {
    std::vector<Address> address_vec;
    LSF_ASSERT(System::GetAllAddress(address_vec));

    for (auto& address : address_vec) {
        std::cout << (address.IsV4() ? "ipv4: " : "ipv6: ") << address.ToString() << std::endl;
    }

    std::cout << "external ipv4 ip: " << System::GetExternalAddress().ToString() << std::endl;
    std::cout << "internal ipv4 ip: " << System::GetInternalAddress().ToString() << std::endl;
}

int main(int argc, char** argv) { LSF_TEST_ALL(argc, argv); }

// vim:ts=4:sw=4:et:ft=cpp:
