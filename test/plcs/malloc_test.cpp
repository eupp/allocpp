#include <gtest/gtest.h>

#include <liballocpp/types.hpp>
#include <liballocpp/concepts/concepts.hpp>
#include <liballocpp/plcs/malloc.hpp>

using namespace allocpp;
using namespace allocpp::concepts;

ALLOCPP_CONCEPT_ASSERT((StatelessAllocPolicy<plcs::malloc>));

TEST(malloc_test, test_malloc)
{
    plcs::malloc malloc_plc;

    auto alloc_req = alloc_request<byte*>::builder()
            .set_size(sizeof(int))
            .set_alignment(alignof(int))
            .build();

    auto alloc_rsp = malloc_plc.allocate(alloc_req);
    EXPECT_NE(nullptr, alloc_rsp.ptr());
    EXPECT_EQ(sizeof(int), alloc_rsp.size());
    EXPECT_EQ(alignof(int), alloc_rsp.alignment());
    EXPECT_TRUE(alloc_rsp.extra_empty());

    int* ptr = reinterpret_cast<int*>(alloc_rsp.ptr());
    *ptr = 42;

    EXPECT_EQ(ownership::UNDEFINED, malloc_plc.owns(alloc_rsp.ptr()));
    EXPECT_EQ(ownership::UNDEFINED, malloc_plc.owns(nullptr));

    auto dealloc_req = dealloc_request<byte*>::builder()
            .set_memory_block(alloc_rsp.memory_block())
            .build();
    auto dealloc_rsp = malloc_plc.deallocate(dealloc_req);
}

