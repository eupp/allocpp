#include <gtest/gtest.h>

#include "alloc_traits.hpp"
#include "statistic_policy.hpp"

using namespace alloc_utility;

class simple_statistic_test: public ::testing::Test
{
public:

    typedef basic_statistic<const void*, size_t> statistic;

    statistic stat;
};

TEST_F(simple_statistic_test, test_constuctor)
{
    EXPECT_EQ(0, stat.allocs_count());
    EXPECT_EQ(0, stat.deallocs_count());
    EXPECT_EQ(0, stat.allocated_blocks_count());
    EXPECT_EQ(0, stat.mem_used());
}

TEST_F(simple_statistic_test, test_allocs_count)
{
    stat.register_alloc(nullptr, 1);
    EXPECT_EQ(1, stat.allocs_count());
}

TEST_F(simple_statistic_test, test_deallocs_count)
{
    stat.register_dealloc(nullptr, 1);
    EXPECT_EQ(1, stat.deallocs_count());
}

TEST_F(simple_statistic_test, test_allocated_blocks_count)
{
    stat.register_alloc(nullptr, 1);
    EXPECT_EQ(1, stat.allocated_blocks_count());
    stat.register_dealloc(nullptr, 1);
    EXPECT_EQ(0, stat.allocated_blocks_count());
}

TEST_F(simple_statistic_test, test_mem_used)
{
    stat.register_alloc(nullptr, 1);
    EXPECT_EQ(1, stat.mem_used());
    stat.register_alloc(nullptr, 4);
    EXPECT_EQ(5, stat.mem_used());
    stat.register_dealloc(nullptr, 5);
    EXPECT_EQ(0, stat.mem_used());
}

class statistic_policy_test: public ::testing::Test
{
public:

    typedef statistic_policy<int> allocator;
    typedef typename allocator::statistic_type statistic;

    statistic_policy_test()
    {
        alloc.set_statistic(&stat);
    }

    statistic stat;
    allocator alloc;
};

TEST_F(statistic_policy_test, test_allocate)
{
    alloc.allocate(1, nullptr);
    EXPECT_EQ(1, stat.allocs_count());
    EXPECT_EQ(1, stat.allocated_blocks_count());
    EXPECT_EQ(sizeof(int), stat.mem_used());
    alloc.allocate(4, nullptr);
    EXPECT_EQ(2, stat.allocs_count());
    EXPECT_EQ(2, stat.allocated_blocks_count());
    EXPECT_EQ(5 * sizeof(int), stat.mem_used());
}

TEST_F(statistic_policy_test, test_deallocate)
{
    alloc.allocate(1, nullptr);
    alloc.deallocate(nullptr, 1);
    EXPECT_EQ(1, stat.deallocs_count());
    EXPECT_EQ(0, stat.allocated_blocks_count());
    EXPECT_EQ(0, stat.mem_used());
}


