#include <random>
#include <functional>
#include <stack>

#include <gtest/gtest.h>

#include "allocator.hpp"
#include "alloc_policies.hpp"
#include "pool_allocation.hpp"

using namespace alloc_utility;
using alloc_utility::details::chunk;

class chunk_test: public ::testing::Test
{
public:

    static const int CHUNK_SIZE = chunk<int>::CHUNK_SIZE;

    chunk_test():
        mem(new int[CHUNK_SIZE])
      , chk(mem)
    {
        std::random_device dev;
        std::default_random_engine eng(dev());
        std::bernoulli_distribution distr(0.75);
        rand = [eng, distr] () mutable {return distr(eng); };
    }

    ~chunk_test()
    {
        delete[] mem;
    }

    int* mem;
    chunk<int> chk;
    std::function<bool()> rand;
};

TEST_F(chunk_test, test_is_memory_available)
{
    EXPECT_TRUE(chk.is_memory_available());

    for (int i = 0; i < chk.CHUNK_SIZE; ++i) {
        chk.allocate();
    }
    EXPECT_FALSE(chk.is_memory_available());

    chk.set_pointer(nullptr);
    EXPECT_FALSE(chk.is_memory_available());
}

TEST_F(chunk_test, test_is_owned)
{
    EXPECT_TRUE(chk.is_owned(mem));
    EXPECT_FALSE(chk.is_owned(mem + CHUNK_SIZE + 10));
    EXPECT_FALSE(chk.is_owned(mem - 10));
    EXPECT_FALSE(chk.is_owned(nullptr));
}

TEST_F(chunk_test, test_allocate)
{
    int* ptr1 = chk.allocate();
    EXPECT_NE(nullptr, ptr1);
    EXPECT_TRUE(chk.is_owned(ptr1));

    int* ptr2 = chk.allocate();
    EXPECT_NE(nullptr, ptr2);
    EXPECT_TRUE(chk.is_owned(ptr2));

    for (int i = 0; i < CHUNK_SIZE - 2; ++i) {
        int* ptr3 = chk.allocate();
        // try dereference pointer
        // in case of incorrect allocation it might cause segmentation fault
        *ptr3 = 42;
    }

    int* ptr4 = chk.allocate();
    EXPECT_EQ(nullptr, ptr4);
}

TEST_F(chunk_test, test_deallocate)
{
    int* ptr = nullptr;
    for (int i = 0; i < CHUNK_SIZE; ++i) {
        ptr = chk.allocate();
    }
    chk.deallocate(ptr);
    EXPECT_TRUE(chk.is_memory_available());

    ptr = nullptr;
    ptr = chk.allocate();
    EXPECT_NE(nullptr, ptr);
}

TEST_F(chunk_test, test_stress)
{
    const int ITER_COUNT = 2000;
    std::stack<int*> st;
    for (int i = 0; i < ITER_COUNT; ++i) {
        if (rand() && chk.is_memory_available()) {
            int* ptr = chk.allocate();
            ASSERT_NE(nullptr, ptr);
            // try dereference pointer
            // in case of incorrect allocation it might cause segmentation fault
            *ptr = 42;
            st.push(ptr);
        }
        else if (!st.empty()) {
            int* ptr = st.top();
            st.pop();
            ASSERT_TRUE(chk.is_owned(ptr));
            chk.deallocate(ptr);
            ASSERT_TRUE(chk.is_memory_available());
        }
    }
}
