#include <type_traits>

#include <gtest/gtest.h>

#include "test_types.hpp"

#include "details/policies_list.hpp"

using namespace alloc_utility;
using namespace alloc_utility::details;


class policies_list_test: public ::testing::Test
{
public:
    typedef well_defined_policy<int> int_well_defined_policy;
    typedef poorly_defined_policy<int> int_poorly_defined_policy;
    typedef policies_list<allocation_traits<int>, int_poorly_defined_policy, int_well_defined_policy> ps_list_t;

    ps_list_t ps_list1;
    ps_list_t ps_list2;
};

TEST_F(policies_list_test, test_comparison_ops)
{
    ps_list1.flag = ps_list2.flag = true;
    EXPECT_TRUE(ps_list1.operator==(ps_list2));
    EXPECT_FALSE(ps_list1.operator!=(ps_list2));
    ps_list2.flag = false;
    EXPECT_FALSE(ps_list1.operator==(ps_list2));
    EXPECT_TRUE(ps_list1.operator!=(ps_list2));
}
