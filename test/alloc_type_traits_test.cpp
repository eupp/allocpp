#include <type_traits>
#include <array>

#include <gtest/gtest.h>

#include "test_types.hpp"

#include "alloc_type_traits.hpp"

using namespace alloc_utility;

TEST(alloc_type_traits_test, test_has_rebind)
{
    EXPECT_TRUE(has_rebind<class_with_rebind>::value);
    EXPECT_FALSE(has_rebind<class_without_rebind>::value);
}
