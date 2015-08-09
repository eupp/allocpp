#include <iostream>
#include <type_traits>

#include <gtest/gtest.h>

#include "test_types.hpp"

#include "allocator.hpp"
#include "alloc_traits.hpp"
#include "details/rebind.hpp"

#include "pool_allocation.hpp"
#include "alloc_policies.hpp"
#include "statistic_policy.hpp"

using namespace alloc_utility;
using namespace alloc_utility::details;

TEST(rebind_test, test_enable_rebind)
{
    bool is_same1 = std::is_same<
                                 typename enable_rebind<class_with_rebind, int>::type,
                                 class_with_rebind::rebind<int>
                                >::value;
    EXPECT_TRUE(is_same1);

    bool is_same2 = std::is_same<
                                 typename enable_rebind<class_without_rebind, int>::type,
                                 class_without_rebind
                                >::value;
    EXPECT_TRUE(is_same2);
}

TEST(rebind_test, test_rebind)
{
    typedef allocator<
                        char, class_with_rebind::rebind<char>, class_without_rebind
                     > rebinded_allocator;

    bool is_same = std::is_same<
                                   rebind<allocator, char, class_with_rebind, class_without_rebind>,
                                   rebinded_allocator
                               >::value;
    EXPECT_TRUE(is_same);
}

TEST(rebind_test, test_alloc_rebind)
{
    typedef pool_allocation_policy<int, allocation_traits<int>,
                                        default_allocation_policy<int, allocation_traits<int>,
                                            statistic_policy<int>
                                        >
                                  > int_allocator;

    typedef pool_allocation_policy<char, allocation_traits<char>,
                                        default_allocation_policy<char, allocation_traits<char>,
                                            statistic_policy<char>
                                        >
                                  > char_allocator;

    bool is_same = std::is_same<int_allocator::rebind<char>, char_allocator>::value;
    EXPECT_TRUE(is_same);
}
