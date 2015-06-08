// File:        test_variant.cpp
// Description: ---
// Notes:       ---
// Author:      leoxiang <leoxiang727@qq.com>
// Revision:    2012-11-12 by leoxiang

#include <string>
#include <iostream>
#include "lsf/container/variant.hpp"
#include "lsf/basic/unit_test.hpp"

using namespace std;
using namespace lsf::container;

// check the destructor is properly called
static size_t NodeConstructTime = 0;
static size_t NodeDestructTime = 0;

struct Node
{
    Node(int _data) : data(_data) { NodeConstructTime++; }
    Node(Node const & rhs) : data(rhs.data) { NodeConstructTime++; }
    Node & operator=(Node const & rhs) {
        if (this == &rhs) return *this;
        data = rhs.data;
        return *this;
    }

    ~Node() { NodeDestructTime++; }

    bool operator==(Node const & rhs) const { return data == rhs.data; }
    bool operator!=(Node const & rhs) const { return data != rhs.data; }

    int data;
};

struct TestVisitor : public Functor<bool>
{
    bool operator()(int & i)            { return i == 32; }
    bool operator()(float & f)          { return f == float(2.2); }
    bool operator()(char & c)           { return c == 'c'; }
    bool operator()(char const * & s)   { return s == string("string"); }
};

LSF_TEST_CASE(test_common)
{
    // test variant
    typedef Variant<int, float, char, char const *> var_type;
    var_type v;
    var_type const & const_v = v;

    LSF_ASSERT(v.IsNull());
    LSF_ASSERT(v.Index() == 0);

    v = 32;
    LSF_ASSERT(v.Index() == 0);
    LSF_ASSERT(v.Get<int>() == 32);
    LSF_ASSERT(const_v.Get<int>() == 32);
    LSF_ASSERT(*v.GetPtr<int>() == 32);
    LSF_ASSERT(*const_v.GetPtr<int>() == 32);

    v = 2.2;
    LSF_ASSERT(v.Index() == 1);
    LSF_ASSERT(v.Get<float>() == float(2.2));
    LSF_ASSERT(const_v.Get<float>() == float(2.2));
    LSF_ASSERT(*v.GetPtr<float>() == float(2.2));
    LSF_ASSERT(*const_v.GetPtr<float>() == float(2.2));

    v = 'c';
    LSF_ASSERT(v.Index() == 2);
    LSF_ASSERT(v.Get<char>() == 'c');
    LSF_ASSERT(const_v.Get<char>() == 'c');
    LSF_ASSERT(*v.GetPtr<char>() == 'c');
    LSF_ASSERT(*const_v.GetPtr<char>() == 'c');

    v = "string";
    LSF_ASSERT(v.Index() == 3);
    LSF_ASSERT(v.Get<char const *>()            == string("string"));
    LSF_ASSERT(const_v.Get<char const *>()      == string("string"));
    LSF_ASSERT(*v.GetPtr<char const *>()        == string("string"));
    LSF_ASSERT(*const_v.GetPtr<char const *>()  == string("string"));
}

LSF_TEST_CASE(test_type)
{
    LSF_ASSERT(NodeConstructTime == NodeDestructTime);

    typedef Variant<int, Node> var_type;
    var_type v;
    var_type const & const_v = v;
    int i = 1;
    const int cont_i = i;
    Node node = Node(1);
    const Node const_node = node;

    v = cont_i;         // assign with const ref
    v = i;              // assign with ref
    LSF_ASSERT(v.Index() == 0);
    LSF_ASSERT(v.Get<int>() == i);
    LSF_ASSERT(const_v.Get<int>() == i);
    LSF_ASSERT(*v.GetPtr<int>() == i);
    LSF_ASSERT(*const_v.GetPtr<int>() == i);

    v = const_node;     // assign with const ref
    v = node;           // assign with ref
    LSF_ASSERT(v.Index() == 1);
    LSF_ASSERT(v.Get<Node>() == node);
    LSF_ASSERT(const_v.Get<Node>() == node);
    LSF_ASSERT(*v.GetPtr<Node>() == node);
    LSF_ASSERT(*const_v.GetPtr<Node>() == node);
}

LSF_TEST_CASE(test_const_type)
{
    LSF_ASSERT(NodeConstructTime == NodeDestructTime);

    typedef Variant<const int, const Node> var_type;
    var_type v;
    var_type const & const_v = v;
    int i = 1;
    const int cont_i = i;
    Node node = Node(1);
    const Node const_node = node;

    v = cont_i;         // assign with const ref
    v = i;              // assign with ref
    LSF_ASSERT(v.Index() == 0);
    LSF_ASSERT(v.Get<const int>() == i);
    LSF_ASSERT(const_v.Get<const int>() == i);
    LSF_ASSERT(*v.GetPtr<const int>() == i);
    LSF_ASSERT(*const_v.GetPtr<const int>() == i);

    v = const_node;     // assign with const ref
    v = node;           // assign with ref
    LSF_ASSERT(v.Index() == 1);
    LSF_ASSERT(v.Get<const Node>() == node);
    LSF_ASSERT(const_v.Get<const Node>() == node);
    LSF_ASSERT(*v.GetPtr<const Node>() == node);
    LSF_ASSERT(*const_v.GetPtr<const Node>() == node);
}

LSF_TEST_CASE(test_pointer)
{
    LSF_ASSERT(NodeConstructTime == NodeDestructTime);

    typedef Variant<int *, Node *> var_type;
    var_type v;
    var_type const & const_v = v;
    int i = 1;
    Node node = Node(1);

    v = (int *)NULL;    // assign with NULL
    v = &i;             // assign with ptr
    LSF_ASSERT(v.Index() == 0);
    LSF_ASSERT(*v.Get<int *>() == i);
    LSF_ASSERT(*const_v.Get<int *>() == i);
    LSF_ASSERT(**v.GetPtr<int *>() == i);
    LSF_ASSERT(**const_v.GetPtr<int *>() == i);
    *v.Get<int *>() = 100;
    LSF_ASSERT(i == 100);

    v = (Node *)NULL;   // assign with NULL
    v = &node;          // assign with ptr
    LSF_ASSERT(v.Index() == 1);
    LSF_ASSERT(*v.Get<Node *>() == node);
    LSF_ASSERT(*const_v.Get<Node *>() == node);
    LSF_ASSERT(**v.GetPtr<Node *>() == node);
    LSF_ASSERT(**const_v.GetPtr<Node *>() == node);
    *v.Get<Node *>() = Node(100);
    LSF_ASSERT(node == Node(100));
}

LSF_TEST_CASE(test_const_pointer)
{
    LSF_ASSERT(NodeConstructTime == NodeDestructTime);

    typedef Variant<int const *, Node const *> var_type;
    var_type v;
    var_type const & const_v = v;
    int i = 1;
    int const * ptr_i = &i;
    Node node = Node(1);
    Node const * ptr_node = &node;

    v = (int *)NULL;    // assign with NULL
    v = ptr_i;          // assign with const ptr
    v = &i;             // assign with ptr
    LSF_ASSERT(v.Index() == 0);
    LSF_ASSERT(*v.Get<int const *>() == i);
    LSF_ASSERT(*const_v.Get<int const *>() == i);
    LSF_ASSERT(**v.GetPtr<int const *>() == i);
    LSF_ASSERT(**const_v.GetPtr<int const *>() == i);

    v = (Node *)NULL;   // assign with NULL
    v = ptr_node;       // assign with const ptr
    v = &node;          // assign with ptr
    LSF_ASSERT(v.Index() == 1);
    LSF_ASSERT(*v.Get<Node const *>() == node);
    LSF_ASSERT(*const_v.Get<Node const *>() == node);
    LSF_ASSERT(**v.GetPtr<Node const *>() == node);
    LSF_ASSERT(**const_v.GetPtr<Node const *>() == node);
}

LSF_TEST_CASE(test_void)
{
    LSF_ASSERT(NodeConstructTime == NodeDestructTime);

    typedef Variant<void *, void const *> var_type;
    var_type v;

    v = (void *)NULL;
    LSF_ASSERT(v.Index() == 0);
    LSF_ASSERT(v.Get<void *>() == NULL);

    v = (void const *)NULL;
    LSF_ASSERT(v.Index() == 1);
    LSF_ASSERT(v.Get<void const *>() == NULL);
}

LSF_TEST_CASE(test_visitor)
{
    // test variant
    typedef Variant<int, float, char, char const *> var_type;
    var_type v;

    LSF_ASSERT(v.IsNull());
    LSF_ASSERT(v.Index() == 0);

    v = 32;
    LSF_ASSERT(v.Index() == 0);
    LSF_ASSERT(v.Get<int>() == 32);
    LSF_ASSERT(v.ApplyVisit(TestVisitor()));

    v = 2.2;
    LSF_ASSERT(v.Index() == 1);
    LSF_ASSERT(v.Get<float>() == float(2.2));
    LSF_ASSERT(v.ApplyVisit(TestVisitor()));

    v = 'c';
    LSF_ASSERT(v.Index() == 2);
    LSF_ASSERT(v.Get<char>() == 'c');
    LSF_ASSERT(v.ApplyVisit(TestVisitor()));

    v = "string";
    LSF_ASSERT(v.Index() == 3);
    LSF_ASSERT(v.Get<char const *>() == string("string"));
    LSF_ASSERT(v.ApplyVisit(TestVisitor()));
}

int main(int argc, char **argv)
{
	LSF_TEST_ALL();
}

// vim:ts=4:sw=4:et:ft=cpp:
