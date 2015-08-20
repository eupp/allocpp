#include <gtest/gtest.h>

#include <memory>

#include "test_types.hpp"

#include "pointer_concepts.hpp"

using namespace alloc_utility;
using namespace alloc_utility::pointer_concepts;

TEST(pointer_concepts_test, test_single_object_ptr_trait)
{
    EXPECT_TRUE(single_object_ptr_trait<std::shared_ptr<int>>::value);
    EXPECT_FALSE(single_object_ptr_trait<empty_class>::value);
}

