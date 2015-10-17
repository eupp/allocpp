#include <gtest/gtest.h>

#include <memory>

#include "test_types.hpp"

#include "concepts/pointer_concepts.hpp"
#include "concepts/alloc_policy_concept.hpp"

using namespace alloc_utility;
using namespace alloc_utility::concepts;

TEST(concepts_test, test_is_alloc_policy)
{
    EXPECT_TRUE(is_alloc_policy<well_defined_policy<int>>::value);
    EXPECT_TRUE(is_alloc_policy<poorly_defined_policy<int>>::value);
    EXPECT_FALSE(is_alloc_policy<empty_class>::value);
}

TEST(concepts_test, test_is_single_object_ptr)
{
    EXPECT_TRUE(is_single_object_ptr<int*>::value);
    EXPECT_TRUE(is_single_object_ptr<std::shared_ptr<int>>::value);
    EXPECT_FALSE(is_single_object_ptr<empty_class>::value);
}

TEST(concepts_test, test_is_array_ptr)
{
    EXPECT_TRUE(is_array_ptr<int*>::value);
    EXPECT_FALSE(is_array_ptr<std::shared_ptr<int>>::value);
}

TEST(concepts_test, test_is_random_access_ptr)
{
    EXPECT_TRUE(is_random_access_ptr<int*>::value);
    // EXPECT_FALSE()
}

TEST(concepts_test, test_is_raw_memory_ptr)
{
    EXPECT_TRUE(is_raw_memory_ptr<int*>::value);
}
