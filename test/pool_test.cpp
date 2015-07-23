#include <random>
#include <functional>
#include <stack>

#include <gtest/gtest.h>

#include "allocator.hpp"
#include "alloc_policies.hpp"
#include "pool_allocation.hpp"

using namespace alloc_utility;
using alloc_utility::details::chunk;

class bernulli_generator
{
public:

    explicit bernulli_generator(double p = 0.5):
        m_eng(std::random_device().operator()())
      , m_distr(p)
    {}

    bool operator()()
    {
        return m_distr(m_eng);
    }

private:

    std::default_random_engine m_eng;
    std::bernoulli_distribution m_distr;
};

class chunk_test: public ::testing::Test
{
public:

    static const int CHUNK_SIZE = chunk<int>::CHUNK_SIZE;

    chunk_test():
        mem(new int[CHUNK_SIZE])
      , chk(mem)
      , rand(0.75)
    {}

    ~chunk_test()
    {
        delete[] mem;
    }

    int* mem;
    chunk<int> chk;
    bernulli_generator rand;
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
    EXPECT_FALSE(chk.is_owned(mem - 1));
    EXPECT_FALSE(chk.is_owned(nullptr));
}

TEST_F(chunk_test, test_allocate)
{
    int* ptr1 = chk.allocate();
    EXPECT_NE(nullptr, ptr1);
    EXPECT_TRUE(chk.is_owned(ptr1));
    // try dereference pointer
    // in case of incorrect allocation it might cause segmentation fault
    *ptr1 = 42;

    int* ptr2 = chk.allocate();
    EXPECT_NE(nullptr, ptr2);
    EXPECT_NE(ptr1, ptr2);
    EXPECT_TRUE(chk.is_owned(ptr2));
    *ptr2 = 42;

    for (int i = 0; i < CHUNK_SIZE - 2; ++i) {
        int* ptr3 = chk.allocate();
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
        } else if (!st.empty()) {
            int* ptr = st.top();
            st.pop();
            ASSERT_TRUE(chk.is_owned(ptr));
            chk.deallocate(ptr);
            ASSERT_TRUE(chk.is_memory_available());
        }
    }
}

class memory_pool_test: public ::testing::Test
{
public:

    typedef default_allocation_policy<int> default_policy;
    typedef details::memory_pool<int, default_policy> pool_type;

    static const int CHUNK_SIZE = chunk<int>::CHUNK_SIZE;

    memory_pool_test():
        pool(&alloc)
      , rand(0.75)
    {}

    default_policy alloc;
    pool_type pool;
    bernulli_generator rand;
};

TEST_F(memory_pool_test, test_allocate)
{
    int* ptr1 = pool.allocate(1, nullptr);
    EXPECT_NE(nullptr, ptr1);
    // try dereference pointer
    // in case of incorrect allocation it might cause segmentation fault
    *ptr1 = 42;

    for (int i = 0; i < CHUNK_SIZE - 1; ++i) {
        int* ptr2 = pool.allocate(1, nullptr);
        *ptr2 = 42;
    }

    int* ptr3 = pool.allocate(1, nullptr);
    EXPECT_NE(nullptr, ptr3);
    *ptr3 = 42;

    int* ptr4 = pool.allocate(1, ptr3);
    EXPECT_EQ(ptr3, ptr4);

    int* ptr5 = pool.allocate(2, nullptr);
    EXPECT_NE(nullptr, ptr5);
    ptr5[0] = ptr5[1] = 42;
    alloc.deallocate(ptr5, 2);
}

TEST_F(memory_pool_test, test_deallocate)
{
    int* ptr1 = pool.allocate(1, nullptr);
    pool.deallocate(ptr1, 1);

    int* ptr2 = pool.allocate(2, nullptr);
    pool.deallocate(ptr2, 2);

    pool.deallocate(nullptr, 1);
}

TEST_F(memory_pool_test, test_stress)
{
    const int ITER_COUNT = 2000;
    std::stack<int*> st;
    for (int i = 0; i < ITER_COUNT; ++i) {
        if (rand()) {
            int* ptr = pool.allocate(1, nullptr);
            ASSERT_NE(nullptr, ptr);
            // try dereference pointer
            // in case of incorrect allocation it might cause segmentation fault
            *ptr = 42;
            st.push(ptr);
        } else if (!st.empty()) {
            int* ptr = st.top();
            st.pop();
            pool.deallocate(ptr, 1);
        }
    }
}
