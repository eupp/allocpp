#include <gtest/gtest.h>

#include <memory>

#include <liballocpp/types.hpp>
#include <liballocpp/concepts/concepts.hpp>
#include <liballocpp/plcs/bitmap_pool_layout.hpp>

using namespace allocpp;
using namespace allocpp::ptrs;
using namespace allocpp::concepts;

namespace {
typedef plcs::bitmap_pool_layout<byte*> layout_type;

ALLOCPP_CONCEPT_ASSERT((MemoryLayoutPolicy<layout_type>));

struct bitmap_pool_layout_test : public ::testing::Test
{
    static const size_t POOL_SIZE = 8;

    bitmap_pool_layout_test()
        : m_mem(new byte[POOL_SIZE])
        , m_req(alloc_request<byte*>::builder()
                        .set_size(1)
                        .set_alignment(0)
                        .build())
        , m_layout(block_ptr<byte*>(m_mem.get(), POOL_SIZE, 0), m_req)
    {}

    std::unique_ptr<byte[]> m_mem;
    alloc_request<byte*> m_req;
    layout_type m_layout;
};

TEST_F(bitmap_pool_layout_test, test_block_size)
{
    size_t block_size = layout_type::block_size(m_req);
    ASSERT_LE(m_req.size(), block_size);
    ASSERT_LE(POOL_SIZE, block_size);
}

TEST_F(bitmap_pool_layout_test, test_alloc_is_feasible)
{
    ASSERT_TRUE(m_layout.is_feasible(m_req));
}

TEST_F(bitmap_pool_layout_test, test_allocate_1)
{
    auto rsp = m_layout.allocate(m_req);
    ASSERT_NE(nullptr, rsp.ptr());
    ASSERT_EQ(1, rsp.size());
    ASSERT_EQ(0, rsp.alignment());
    byte* p = rsp.ptr();
    *p = 42;
}

TEST_F(bitmap_pool_layout_test, test_allocate_2)
{
    for (size_t i = 0; i < POOL_SIZE; ++i) {
        m_layout.allocate(m_req);
    }
    ASSERT_FALSE(m_layout.is_feasible(m_req));
}

TEST_F(bitmap_pool_layout_test, test_dealloc_is_feasible)
{
    auto rsp = m_layout.allocate(m_req);
    auto dealloc_req = dealloc_request<byte*>::builder()
            .set_memory_block(rsp.memory_block())
            .build();
    ASSERT_TRUE(m_layout.is_feasible(dealloc_req));
}

TEST_F(bitmap_pool_layout_test, test_deallocate)
{
    auto rsp = m_layout.allocate(m_req);
    for (size_t i = 0; i < POOL_SIZE - 1; ++i) {
        m_layout.allocate(m_req);
    }
    auto dealloc_req = dealloc_request<byte*>::builder()
            .set_memory_block(rsp.memory_block())
            .build();
    auto dealloc_rsp = m_layout.deallocate(dealloc_req);
}

}