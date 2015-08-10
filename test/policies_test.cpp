#include <gtest/gtest.h>

#include "allocator.hpp"
#include "alloc_policies.hpp"

using namespace alloc_utility;

TEST(policies_test, test_logging)
{
    typedef allocator<int, allocation_traits<int>,
                      default_allocation_policy<int>,
                      logging_policy<int>
                     > logging_allocator;
    logging_allocator alloc;
    alloc.set_log(&std::cout);
    std::vector<int, logging_allocator> vec(alloc);
    for (int i = 0; i < 1000; ++i) {
        vec.push_back(i);
    }
}

TEST(policies_test, test_bad_alloc_exc)
{
    typedef allocator<int, allocation_traits<int>,
                      throw_bad_alloc_policy<int>
                     > throwing_allocator;
    throwing_allocator alloc;
    EXPECT_THROW(alloc.allocate(1), std::bad_alloc);
}

