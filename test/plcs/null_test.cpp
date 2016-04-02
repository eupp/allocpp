#include <gtest/gtest.h>

#include <liballocpp/types.hpp>
#include <liballocpp/concepts/concepts.hpp>
#include <liballocpp/plcs/null.hpp>

using namespace allocpp;
using namespace allocpp::plcs;
using namespace allocpp::concepts;

ALLOCPP_CONCEPT_ASSERT((StatelessAllocPolicy<null<byte*>>));

TEST(null_plc_test, test_null_plc)
{
    typedef null<byte*> null_plc;

    null_plc plc;

    auto alloc_rqst = alloc_request<byte*>::builder().set_size(42).set_alignment(8).build();
    alloc_response<byte*> alloc_rsp = plc.allocate(alloc_rqst);
    EXPECT_EQ(nullptr, alloc_rsp.ptr());
    EXPECT_EQ(0, alloc_rsp.size());
    EXPECT_EQ(0, alloc_rsp.alignment());
    EXPECT_TRUE(alloc_rsp.extra_empty());

    auto dealloc_rqst = dealloc_request<byte*>::builder().set_memory_block(nullptr, 0).build();
    auto dealloc_rsp = plc.deallocate(dealloc_rqst);
    EXPECT_TRUE(alloc_rsp.extra_empty());

    byte v;
    EXPECT_FALSE(plc.owns(nullptr));
    EXPECT_FALSE(plc.owns(&v));

    EXPECT_FALSE(plc.is_available(alloc_rqst));
}