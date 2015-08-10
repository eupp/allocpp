#include <type_traits>

#include <gtest/gtest.h>

#include "test_types.hpp"

#include "alloc_type_traits.hpp"

using namespace alloc_utility;

TEST(alloc_type_traits_test, test_has_rebind)
{
    EXPECT_TRUE(has_rebind<class_with_rebind>::value);
    EXPECT_FALSE(has_rebind<empty_class>::value);
}

TEST(alloc_type_traits_test, test_has_equal_to_op)
{
    EXPECT_TRUE(has_equal_to_op<class_with_comparison_ops>::value);
    EXPECT_FALSE(has_equal_to_op<empty_class>::value);
}

TEST(alloc_type_traits_test, test_has_not_equal_to_op)
{
    EXPECT_TRUE(has_not_equal_to_op<class_with_comparison_ops>::value);
    EXPECT_FALSE(has_not_equal_to_op<empty_class>::value);
}
