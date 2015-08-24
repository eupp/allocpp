#include <gtest/gtest.h>

#include <memory>
#include <cstddef>
#include <vector>

#include "test_types.hpp"

#include "details/alloc_type_traits.hpp"

using namespace alloc_utility;
using namespace alloc_utility::details;

typedef typename std::vector<int>::iterator iterator_type;

TEST(alloc_type_traits_test, test_has_rebind)
{
    EXPECT_TRUE(has_rebind<class_with_rebind>::value);
    EXPECT_FALSE(has_rebind<empty_class>::value);
}

TEST(alloc_type_traits_test, test_has_dereference_operator)
{
    EXPECT_TRUE(has_dereference_operator<int*>::value);
    EXPECT_TRUE(has_dereference_operator<std::shared_ptr<int>>::value);
    EXPECT_FALSE(has_dereference_operator<empty_class>::value);
    EXPECT_FALSE(has_dereference_operator<int>::value);
}

TEST(alloc_type_traits_test, test_has_member_access_operator)
{
    EXPECT_TRUE(has_member_access_operator<std::shared_ptr<int>>::value);
    EXPECT_FALSE(has_member_access_operator<empty_class>::value);
    EXPECT_FALSE(has_member_access_operator<int>::value);
}

TEST(alloc_type_traits_test, test_has_array_subscript_operator)
{
    EXPECT_TRUE(has_array_subscript_operator<int*>::value);
    EXPECT_TRUE(has_array_subscript_operator<std::unique_ptr<int[]>>::value);
    EXPECT_FALSE(has_array_subscript_operator<empty_class>::value);
    EXPECT_FALSE(has_array_subscript_operator<int>::value);
}

TEST(alloc_type_traits_test, test_has_pre_increment_operator)
{
    EXPECT_TRUE(has_pre_increment_operator<int*>::value);
    EXPECT_TRUE(has_pre_increment_operator<iterator_type>::value);
    EXPECT_FALSE(has_pre_increment_operator<empty_class>::value);
}

TEST(alloc_type_traits_test, test_has_pre_decrement_operator)
{
    EXPECT_TRUE(has_pre_decrement_operator<int*>::value);
    EXPECT_TRUE(has_pre_decrement_operator<iterator_type>::value);
    EXPECT_FALSE(has_pre_decrement_operator<empty_class>::value);
}

TEST(alloc_type_traits_test, test_has_post_increment_operator)
{
    EXPECT_TRUE(has_post_increment_operator<int*>::value);
    EXPECT_TRUE(has_post_increment_operator<iterator_type>::value);
    EXPECT_FALSE(has_post_increment_operator<empty_class>::value);
}

TEST(alloc_type_traits_test, test_has_post_decrement_operator)
{
    EXPECT_TRUE(has_post_decrement_operator<int*>::value);
    EXPECT_TRUE(has_post_decrement_operator<iterator_type>::value);
    EXPECT_FALSE(has_post_decrement_operator<empty_class>::value);
}

TEST(alloc_type_traits_test, test_supports_equality)
{
    bool value1 = supports_equality<class_with_comparison_ops, class_with_comparison_ops>::value;
    EXPECT_TRUE(value1);

    bool value2 = supports_equality<empty_class, empty_class>::value;
    EXPECT_FALSE(value2);
}

TEST(alloc_type_traits_test, test_supports_inequality)
{
    bool value1 = supports_inequality<class_with_comparison_ops, class_with_comparison_ops>::value;
    EXPECT_TRUE(value1);

    bool value2 = supports_inequality<empty_class, empty_class>::value;
    EXPECT_FALSE(value2);
}

TEST(alloc_type_traits_test, test_is_swappable)
{
    bool value1 = is_swappable<int, int>::value;
    EXPECT_TRUE(value1);

    bool value2 = is_swappable<adl_check::swappable, adl_check::swappable>::value;
    EXPECT_TRUE(value2);

    bool value3 = is_swappable<adl_check::not_swappable, adl_check::not_swappable>::value;
    EXPECT_FALSE(value3);
}

TEST(alloc_type_traits_test, test_is_constrictible_from_rebind)
{
    EXPECT_TRUE(is_constructible_from_rebind<well_defined_policy<int>>::value);
    EXPECT_FALSE(is_constructible_from_rebind<class_with_rebind>::value);
    EXPECT_FALSE(is_constructible_from_rebind<empty_class>::value);
}

TEST(alloc_type_traits_test, test_enable_propagate)
{
    EXPECT_TRUE(enable_propagate_on_copy<well_defined_policy<int>>::value);
    EXPECT_FALSE(enable_propagate_on_copy<poorly_defined_policy<int>>::value);

    EXPECT_TRUE(enable_propagate_on_move<well_defined_policy<int>>::value);
    EXPECT_FALSE(enable_propagate_on_move<poorly_defined_policy<int>>::value);

    EXPECT_TRUE(enable_propagate_on_swap<well_defined_policy<int>>::value);
    EXPECT_FALSE(enable_propagate_on_swap<poorly_defined_policy<int>>::value);
}

TEST(alloc_type_traits_test, test_enable_difference_type)
{
    bool value1 = std::is_same<
        typename std::pointer_traits<int*>::difference_type,
        typename enable_difference_type<int*>::type
        >::value;
    EXPECT_TRUE(value1);

    bool value2 = std::is_same<
        std::ptrdiff_t,
        typename enable_difference_type<int>::type
        >::value;
    EXPECT_TRUE(value2);
}
