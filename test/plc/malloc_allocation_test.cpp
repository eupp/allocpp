#include <gtest/gtest.h>

#include "macro_alloc.hpp"
#include "plc/allocation_mechanism.hpp"
#include "plc/malloc_mechanism.hpp"
#include "plc/none.hpp"
#include "allocation_traits.hpp"

using namespace alloc_utility;
using namespace alloc_utility::plc;

class malloc_allocation_test: public ::testing::Test
{
public:


    typedef allocation_void_traits<void*> traits_t;
    typedef malloc_allocation<traits_t, none<traits_t>> malloc_allocation_t;

    DECLARE_ALLOC_REQUEST_RESPONSE(traits_t)

    malloc_allocation_test()
    {
        alloc_request.set_obj_size(4);
        alloc_request.set_obj_count(1);
    }

    malloc_allocation_t allocator;
    allocation_request_t alloc_request;
    deallocation_request_t dealloc_request;
};

TEST_F(malloc_allocation_test, test_allocate)
{
    allocation_response_t response = allocator.allocate(alloc_request);

    void* ptr = response.pointer();
    EXPECT_NE(nullptr, ptr);
    EXPECT_EQ(alloc_request.size(), response.size());

    if (ptr) {
        int* iptr = reinterpret_cast<int*>(ptr);
        *iptr = 42;
    }

    free(ptr);
}

TEST_F(malloc_allocation_test, test_deallocate)
{
    allocation_response_t alloc_response = allocator.allocate(alloc_request);
    dealloc_request.set_pointer(alloc_response.pointer());
    deallocation_response_t dealloc_response = allocator.deallocate(dealloc_request);
    EXPECT_TRUE(dealloc_response.succeeded());
}
