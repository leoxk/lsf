// File:        test_config.cpp
// Description: ---
// Notes:       ---
// Author:      leoxiang <leoxiang727@qq.com>
// Revision:    2011-09-17 by leoxiang

#include "lsf/basic/unit_test.hpp"
#include "lsf/util/config.hpp"

using namespace std;
using namespace lsf::util;

static string conf_path;

LSF_TEST_CASE(parse_from_file) {
    cout << strtoul("", nullptr, 0) << endl;
    // declare a instance and load file
    Config cf(conf_path);

    LSF_ASSERT(cf.Get("host") == "192.168.0.1");
    LSF_ASSERT(cf.Get("user") == "leoxiang");
    LSF_ASSERT(cf.Get("passwd") == "123456");
    LSF_ASSERT(cf.Get("port") == "22");
    LSF_ASSERT(cf.Get("dbname") == "-123");

    // clear and reload
    cf.Clear();
    LSF_ASSERT(cf.size() == 0);

    LSF_ASSERT(cf.ParseFromFile(conf_path));
    LSF_ASSERT(cf.Get("host") == "192.168.0.1");
    LSF_ASSERT(cf.Get("user") == "leoxiang");
    LSF_ASSERT(cf.Get("passwd") == "123456");
    LSF_ASSERT(cf.Get("port") == "22");
    LSF_ASSERT(cf.Get("dbname") == "-123");

    // module
    LSF_ASSERT(cf.Get("module1", "key1") == "val1");
    LSF_ASSERT(cf.Get("module1", "key2") == "val2");
    LSF_ASSERT(cf.Get("module1", "key3") == "val3");

    LSF_ASSERT(cf.Get("module2", "key1") == "val1");
    LSF_ASSERT(cf.Get("module2", "key2") == "val2");
    LSF_ASSERT(cf.Get("module2", "key3") == "val3");

    cf.PrintAll(cout);
}

LSF_TEST_CASE(parse_from_string) {
    Config cf;

    // parse config from string
    LSF_ASSERT(cf.ParseFromString("my_option = my_arg"));
    LSF_ASSERT(cf.Get("my_option") == "my_arg");

    // more
    LSF_ASSERT(cf.ParseFromString("my_option1 = my_arg1"));
    LSF_ASSERT(cf.ParseFromString("my_option2 = my_arg2"));
    LSF_ASSERT(cf.Get("my_option1") == "my_arg1");
    LSF_ASSERT(cf.Get("my_option2") == "my_arg2");
}

LSF_TEST_CASE(tail_comments) {
    // test tail comments
    Config cf;
    LSF_ASSERT(cf.ParseFromString("my_option3 = my_arg3 # some comments"));
    LSF_ASSERT(cf.Get("my_option3") == "my_arg3");
}

LSF_TEST_CASE(test_type_cast) {
    Config cf(conf_path);

    LSF_ASSERT(cf.Get<uint32_t>("passwd") == 123456);
    LSF_ASSERT(cf.Get<uint32_t>("port") == 22);
    LSF_ASSERT(cf.Get<int32_t>("dbname") == -123);
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

// vim:ts=4:sw=4:
