#include <gtest/gtest.h>

#include <memory>

#include "test_types.hpp"

#include "concepts/pointer_concepts.hpp"

using namespace alloc_utility;
using namespace alloc_utility::concepts;

TEST(pointer_concepts_test, test_is_single_object_ptr)
{
    EXPECT_TRUE(is_single_object_ptr<std::shared_ptr<int>>::value);
    EXPECT_FALSE(is_single_object_ptr<empty_class>::value);
}

