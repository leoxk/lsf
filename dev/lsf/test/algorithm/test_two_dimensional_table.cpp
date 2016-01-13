// File:        test_two_dimensional_table.cpp
// Description: ---
// Notes:       ---
// Author:      leoxiang <leoxiang727@qq.com>
// Revision:    2015-11-09 by leoxiang

#include "lsf/basic/unit_test.hpp"
#include "lsf/algorithm/two_dimensional_table.hpp"

using namespace std;
using namespace lsf::algorithm;
using namespace lsf::basic;

static string conf_path;

LSF_TEST_CASE(test_table_with_int) {
    TwoDimensionalTable<int> table;
    LSF_ASSERT(table.ParseFromFile(conf_path));
    LSF_ASSERT(table.ToString(0) == "0,1,2,3,4,5,6,7,8,9,10,50,0,0,0,0,");
    LSF_ASSERT(table.ToString(1) == "10,5000,3000,1000,142,142,142,142,142,142,142,0,0,0,0,0,");
    LSF_ASSERT(table.ToString(2) == "30,4000,3200,2560,2048,1638,1310,1048,838,671,536,429,0,0,0,0,");
    LSF_ASSERT(table.ToString(3) == "50,4000,3200,2560,2048,1638,1310,1048,838,671,536,429,0,0,0,0,");
}

LSF_TEST_CASE(test_table_with_string) {
    TwoDimensionalTable<std::string> table;
    LSF_ASSERT(table.ParseFromFile(conf_path));
    LSF_ASSERT(table.ToString(0) == "最大人数,1,2,3,4,5,6,7,8,9,10,50,,,,,");
    LSF_ASSERT(table.ToString(1) == "10,5000,3000,1000,142.85714285714286,142.85714285714286,142.85714285714286,142.85714285714286,142.85714285714286,142.85714285714286,142.85714285714286,,,,,,");
    LSF_ASSERT(table.ToString(2) == "30,4000,3200,2560,2048,1638.4,1310.7200000000003,1048.5760000000002,838.86080000000027,671.08864000000028,536.8709120000002,429.49672960000021,,,,,");
    LSF_ASSERT(table.ToString(3) == "50,4000,3200,2560,2048,1638.4,1310.7200000000003,1048.5760000000002,838.86080000000027,671.08864000000028,536.8709120000002,429.49672960000021,,,,,");

    for (size_t i = 0; i < table.size(); ++i) std::cout << table.ToString(i) << std::endl;
}

LSF_TEST_CASE(test_table_with_float) {
    TwoDimensionalTable<float> table;
    LSF_ASSERT(table.ParseFromFile(conf_path));
    LSF_ASSERT(table.ToString(0) == "0,1,2,3,4,5,6,7,8,9,10,50,0,0,0,0,");
    LSF_ASSERT(table.ToString(1) == "10,5000,3000,1000,142.857,142.857,142.857,142.857,142.857,142.857,142.857,0,0,0,0,0,");
    LSF_ASSERT(table.ToString(2) == "30,4000,3200,2560,2048,1638.4,1310.72,1048.58,838.861,671.089,536.871,429.497,0,0,0,0,");
    LSF_ASSERT(table.ToString(3) == "50,4000,3200,2560,2048,1638.4,1310.72,1048.58,838.861,671.089,536.871,429.497,0,0,0,0,");
}

int main(int argc, char **argv) {
    if (argc != 2) {
        cout << "Usage: " << argv[0] << " [conf_path]" << endl;
        exit(0);
    } else {
        conf_path = argv[1];
    }

    LSF_TEST_ALL(argc, argv);
}

// vim:ts=4:sw=4:et:ft=cpp:
