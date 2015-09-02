#include <vector>
#include <list>

#include <gtest/gtest.h>

#include "allocator.hpp"
#include "alloc_traits.hpp"
#include "alloc_policies.hpp"
#include "pool_allocation.hpp"
#include "statistic_policy.hpp"

using namespace alloc_utility;

class stl_test: public ::testing::Test
{
public:

    typedef allocator<int, allocation_traits<int>,
                        statistic_policy<int>,
                        pool_allocation_policy<int>,
                        default_allocation_policy<int>,
                        logging_policy<int>
                     > test_allocator;

    typedef statistic_policy<int>::statistic_type statistic_type;
    typedef allocation_traits<int>::size_type size_type;

    typedef std::vector<int, test_allocator> test_vector;
    typedef std::list<int, test_allocator> test_list;

    static const int CONT_SIZE = 100;
    static const int ELEM_SIZE = sizeof(int);

    stl_test()
    {
        alloc.set_statistic(&stat);
        alloc.set_log(&std::cout);
    }

    statistic_type stat;
    test_allocator alloc;
};

TEST_F(stl_test, test_vector_construct)
{
    test_vector vec1(alloc);
    EXPECT_EQ(alloc, vec1.get_allocator());

    test_vector vec2(2048, 0, alloc);
    EXPECT_EQ(alloc, vec2.get_allocator());

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
    ASSERT_LE(CONT_SIZE * ELEM_SIZE, stat.mem_used());
    // check that all elements are allocated and initialized properly
    for (int i = 0; i < CONT_SIZE; ++i) {
        ASSERT_EQ(0, vec[i]);
    }
}

TEST_F(stl_test, test_vector_shrink)
{
    test_vector vec(alloc);
    vec.reserve(CONT_SIZE);
    vec.resize(1);
    vec.shrink_to_fit();
    EXPECT_LE((size_t)ELEM_SIZE, stat.mem_used());
}
