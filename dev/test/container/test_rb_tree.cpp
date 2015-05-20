// File:        test_rb_tree.cpp
// Description: ---
// Notes:       ---
// Author:      leoxiang <leoxiang727@qq.com>
// Revision:    2012-10-29 by leoxiang

#include <set>
#include <fstream>
#include "lsf/basic/unit_test.hpp"
#include "lsf/basic/type_cast.hpp"
#include "lsf/util/random.hpp"
#include "lsf/container/set.hpp"
#include "node.hpp"

using namespace std;
using namespace lsf::basic;
using namespace lsf::container;
using namespace lsf::util;

static string conf_path;

#define SHM_KEY  0x082157ff

#define PRINT_ALL \
    do { \
        cout << *sets.FindRoot() << endl; \
        cout << sets.ToString(TestNode(1, 1)) << endl; \
        cout << sets.ToString(TestNode(2, 2)) << endl; \
        cout << sets.ToString(TestNode(3, 3)) << endl; \
        cout << sets.ToString(TestNode(4, 4)) << endl; \
        cout << sets.ToString(TestNode(5, 5)) << endl; \
        cout << sets.ToString(TestNode(6, 6)) << endl; \
        cout << sets.ToString(TestNode(7, 7)) << endl; \
        cout << sets.ToString(TestNode(8, 8)) << endl; \
        cout << sets.ToString(TestNode(9, 9)) << endl; \
        cout << sets.ToString(TestNode(10, 10)) << endl; \
    } while (0)

LSF_TEST_CASE(test_load_batch)
{
    ifstream    ifs;
    size_t      line_count = 0;

    // get line count
    ifs.close();
    ifs.open(conf_path.c_str());
    for (string line; !ifs.eof(); getline(ifs, line)) {
        if (line.empty()) continue;
        line_count++;
    }
    
    // init cache
    Set<uint32_t, SharedMem> sets;
    if (SharedMem::IsShmExist(SHM_KEY)) LSF_ASSERT(SharedMem::Delete(SHM_KEY));
    LSF_ASSERT(SharedMem::Create(SHM_KEY, sets.CalcByteSize(line_count)));
    LSF_ASSERT(sets.BindAndInitStorage(SharedMem(SHM_KEY)));
    LSF_ASSERT(sets.MaxSize() == line_count);

    // insert
    ifs.close();
    ifs.open(conf_path.c_str());
    for (string line; !ifs.eof(); getline(ifs, line)) {
        if (line.empty()) continue;
        //if (sets.Size() >= 0 && sets.Size() % (line_count / 10) == 0) {
            //cout << "size: " << sets.Size() << endl;
            //LSF_ASSERT_EXIT(sets.CheckConsist());
        //}
        LSF_ASSERT_EXIT_ERR_ONLY(sets.Insert(TypeCast<uint32_t>(line)));
    }

    LSF_ASSERT(sets.CheckConsist());
    LSF_ASSERT(sets.Size() == line_count);
    
    // erase
    ifs.close();
    ifs.open(conf_path.c_str());
    for (string line; !ifs.eof(); getline(ifs, line)) {
        if (line.empty()) continue;
        if (sets.Size() <= line_count && sets.Size() % (line_count / 10) == 0) {
        //if (sets.Size() <= 77947 && sets.Size() % 1 == 0) {
            cout << "size: " << sets.Size() << endl;
            LSF_ASSERT_EXIT(sets.CheckConsist());
        }
        LSF_ASSERT_EXIT_ERR_ONLY(sets.Erase(TypeCast<uint32_t>(line)));
    }

    LSF_ASSERT(sets.CheckConsist());
    LSF_ASSERT(sets.Size() == 0);
}

int main(int argc, char **argv)
{
    if (argc != 2) {
        cout << "Usage: " << argv[0] << " [conf_path]" << endl;
        exit(0);
    }
    else {
        conf_path = argv[1];
    }

	LSF_TEST_ALL();
}

// vim:ts=4:sw=4:et:ft=cpp:
