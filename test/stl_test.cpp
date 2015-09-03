#include <vector>
#include <list>
#include <algorithm>
#include <functional>
#include <cstdint>

#include <gtest/gtest.h>

#include "allocator.hpp"
#include "alloc_traits.hpp"
#include "alloc_policies.hpp"
#include "pool_allocation.hpp"
#include "statistic_policy.hpp"

using namespace alloc_utility;

class element
{
public:
    explicit element(int a): m_a(2 * a) {}
    friend bool operator==(const element& e1, const element& e2);
private:
    int m_a;
};

bool operator==(const element& e1, const element& e2)
{
    return e1.m_a == e2.m_a;
}

class stl_test: public ::testing::Test
{
public:

    typedef std::uint8_t byte;

    typedef allocator<byte, allocation_traits<byte>,
                        pool_allocation_policy<byte>,
                        default_allocation_policy<byte>,
                        statistic_policy<byte>,
                        logging_policy<byte>
                     > test_allocator;

    typedef typename test_allocator::rebind<element>::other test_allocator_e;


    typedef statistic_policy<byte>::statistic_type statistic_type;
    typedef allocation_traits<byte>::size_type size_type;

    typedef std::vector<byte, test_allocator> test_vector;
    typedef std::list<byte, test_allocator> test_list;

    typedef std::vector<element, test_allocator_e> test_vector_e;

    static const int CONT_SIZE = 100;
    static const int ELEM_SIZE = sizeof(byte);

    // checks that all elements of container equal to value
    template <typename Cont, typename T>
    static bool check_container(const Cont& cont, T val)
    {
        typedef typename Cont::value_type value_type;
        return std::all_of(cont.begin(), cont.end(),
                           std::bind(std::equal_to<value_type>(), std::placeholders::_1, val));
    }

    stl_test()
    {
        alloc.set_statistic(&stat);
        alloc.set_log(&std::cout);

        alloc_e.set_statistic(&stat);
        alloc_e.set_log(&std::cout);
    }

    statistic_type      stat;
    test_allocator      alloc;
    test_allocator_e    alloc_e;
};

TEST_F(stl_test, test_vector_construct)
{
    test_vector vec1(alloc);
    EXPECT_EQ(alloc, vec1.get_allocator());

    test_vector vec2(CONT_SIZE, 0, alloc);
    EXPECT_EQ(alloc, vec2.get_allocator());
    EXPECT_LE(CONT_SIZE * ELEM_SIZE, stat.mem_used());

    test_vector vec3(vec1);
    EXPECT_EQ(alloc, vec3.get_allocator());

    test_vector vec4(std::move(vec3));
    EXPECT_EQ(alloc, vec4.get_allocator());
}

TEST_F(stl_test, test_vector_reserve)
{
    test_vector vec(alloc);
    vec.reserve(CONT_SIZE);
    EXPECT_LE(CONT_SIZE * ELEM_SIZE, stat.mem_used());
}

TEST_F(stl_test, test_vector_resize)
{
    test_vector vec(alloc);
    vec.resize(CONT_SIZE);
    EXPECT_LE(CONT_SIZE * ELEM_SIZE, stat.mem_used());
    // check that all elements are allocated and initialized properly
    EXPECT_TRUE(check_container(vec, 0));
}

TEST_F(stl_test, test_vector_shrink)
{
    test_vector vec(alloc);
    vec.reserve(CONT_SIZE);
    vec.resize(1);
    vec.shrink_to_fit();
    EXPECT_LE((size_t)ELEM_SIZE, stat.mem_used());
}

TEST_F(stl_test, test_vector_assign)
{
    test_vector vec(alloc);
    vec.reserve(1);
    vec.assign(CONT_SIZE, 0);
    EXPECT_LE(CONT_SIZE * ELEM_SIZE, stat.mem_used());
    EXPECT_TRUE(check_container(vec, 0));
}

TEST_F(stl_test, test_vector_insert)
{
    test_vector vec(alloc);
    vec.resize(CONT_SIZE);
    vec.insert(vec.begin() + 1, CONT_SIZE, 0);
    EXPECT_LE(2 * CONT_SIZE * ELEM_SIZE, stat.mem_used());
    EXPECT_TRUE(check_container(vec, 0));
}

TEST_F(stl_test, test_vector_push_back)
{
    test_vector vec(alloc);
    for (int i = 0; i < CONT_SIZE; ++i) {
        vec.push_back(0);
    }
    EXPECT_LE((size_t) CONT_SIZE, stat.mem_used());
    EXPECT_TRUE(check_container(vec, 0));
}

TEST_F(stl_test, test_vector_emplace_back)
{
    test_vector_e vec(alloc_e);
    for (int i = 0; i < CONT_SIZE; ++i) {
        vec.emplace_back(0);
    }
    EXPECT_LE((size_t) CONT_SIZE, stat.mem_used());
    EXPECT_TRUE(check_container(vec, element(0)));
}

TEST_F(stl_test, test_vector_clear)
{
    test_vector vec(alloc);
    vec.resize(CONT_SIZE);
    vec.clear();
    EXPECT_LE((size_t) CONT_SIZE, stat.mem_used());
}

TEST_F(stl_test, test_list_construct)
{
    test_list lst1(alloc);
    EXPECT_EQ(alloc, lst1.get_allocator());

    test_list lst2(1, 0, alloc);
    EXPECT_EQ(alloc, lst2.get_allocator());
    EXPECT_LE((size_t)ELEM_SIZE, stat.mem_used());

    test_list lst3(lst1);
    EXPECT_EQ(alloc, lst3.get_allocator());

    test_list lst4(std::move(lst3));
    EXPECT_EQ(alloc, lst4.get_allocator());
}

TEST_F(stl_test, test_list_resize)
{
    test_list lst(alloc);
    lst.resize(CONT_SIZE);
    EXPECT_LE(CONT_SIZE * ELEM_SIZE, stat.mem_used());
    EXPECT_TRUE(check_container(lst, 0));
}
