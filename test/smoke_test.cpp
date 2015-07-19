#include <vector>

#include <gtest/gtest.h>

#include "allocator.hpp"
#include "alloc_policies.hpp"

using namespace alloc_utility;

struct C
{
    int m_i;

    C(int i):
        m_i(i)
    {}
};

typedef allocator<int, allocation_traits<int>, default_allocation_policy<int>> simple_int_allocator;
typedef allocator<C, allocation_traits<C>, default_allocation_policy<C>> simple_object_allocator;

TEST(smoke_test, test_allocate) {
    simple_int_allocator alloc;
    const int size = 10;
    int* p = alloc.allocate(size);

    EXPECT_NE(p, nullptr);
    for (int i = 0; i < size; ++i) {
        p[i] = i;
    }
    alloc.deallocate(p, size);
}


TEST(smoke_test, test_address)
{
    simple_int_allocator alloc;
    int* p = alloc.allocate(1);

    EXPECT_EQ(p, alloc.address(*p));
    alloc.deallocate(p, 1);
}

TEST(smoke_test, test_construct)
{
    simple_object_allocator alloc;
    C* p = alloc.allocate(1);
    alloc.construct<C, int>(p, 42);

    EXPECT_EQ(42, p->m_i);
    alloc.destroy(p);
    alloc.deallocate(p, 1);
}

TEST(smoke_test, test_max_size)
{
    simple_int_allocator alloc;
    EXPECT_TRUE(alloc.max_size() > 0);
}

TEST(smoke_test, test_stl)
{
    std::vector<int, simple_int_allocator> vec;
    for (int i = 0; i < 1000; ++i) {
        vec.push_back(i);
    }
    vec[0] = 42;
}

