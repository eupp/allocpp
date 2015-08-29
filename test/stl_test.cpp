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
                        pool_allocation_policy<int>,
                        default_allocation_policy<int>,
                        logging_policy<int>,
                        statistic_policy<int>
                     > test_allocator;

    typedef statistic_policy<int>::statistic_type statistic_type;
    typedef allocation_traits<int>::size_type size_type;

    typedef std::vector<int, test_allocator> test_vector;
    typedef std::list<int, test_allocator> test_list;

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
