#include <cstddef>
#include <random>
#include <functional>
#include <stack>

#include <gtest/gtest.h>

#include "allocator.hpp"
#include "alloc_policies.hpp"
#include "pool_allocation.hpp"
#include "statistic_policy.hpp"

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

    static const int CHUNK_SIZE = chunk<std::uint8_t*, size_t>::CHUNK_MAXSIZE;

    chunk_test():
        mem(new std::uint8_t[sizeof(int) * CHUNK_SIZE])
      , chk(mem, sizeof(int))
      , rand(0.75)
    {}

    ~chunk_test()
    {
        delete[] mem;
    }

    std::uint8_t* mem;
    chunk<std::uint8_t*, size_t> chk;
    bernulli_generator rand;
};

TEST_F(chunk_test, test_size)
{
    EXPECT_EQ((int)CHUNK_SIZE, chk.size());
}

TEST_F(chunk_test, test_is_memory_available)
{
    EXPECT_TRUE(chk.is_memory_available());

    for (int i = 0; i < CHUNK_SIZE; ++i) {
        chk.allocate(sizeof(int));
    }
    EXPECT_FALSE(chk.is_memory_available());

    chk.set_pointer(nullptr, sizeof(int));
    EXPECT_FALSE(chk.is_memory_available());
}

TEST_F(chunk_test, test_is_owned)
{
    EXPECT_TRUE(chk.is_owned(mem, sizeof(int)));
    EXPECT_FALSE(chk.is_owned(mem + (CHUNK_SIZE + 10) * sizeof(int), sizeof(int)));
    EXPECT_FALSE(chk.is_owned(mem - 1, sizeof(int)));
    EXPECT_FALSE(chk.is_owned(nullptr, sizeof(int)));
}

TEST_F(chunk_test, test_allocate)
{
    std::uint8_t* ptr1 = chk.allocate(sizeof(int));
    EXPECT_NE(nullptr, ptr1);
    EXPECT_TRUE(chk.is_owned(ptr1, sizeof(int)));
    // try dereference pointer
    // in case of incorrect allocation it might cause segmentation fault
    *ptr1 = 42;

    std::uint8_t* ptr2 = chk.allocate(sizeof(int));
    EXPECT_NE(nullptr, ptr2);
    EXPECT_NE(ptr1, ptr2);
    EXPECT_TRUE(chk.is_owned(ptr2, sizeof(int)));
    *ptr2 = 42;

    for (int i = 0; i < CHUNK_SIZE - 2; ++i) {
        std::uint8_t* ptr3 = chk.allocate(sizeof(int));
        *ptr3 = 42;
    }

    std::uint8_t* ptr4 = chk.allocate(sizeof(int));
    EXPECT_EQ(nullptr, ptr4);
}

TEST_F(chunk_test, test_deallocate)
{
    std::uint8_t* ptr = nullptr;
    for (int i = 0; i < CHUNK_SIZE; ++i) {
        ptr = chk.allocate(sizeof(int));
    }
    chk.deallocate(ptr, sizeof(int));
    EXPECT_TRUE(chk.is_memory_available());

    ptr = nullptr;
    ptr = chk.allocate(sizeof(int));
    EXPECT_NE(nullptr, ptr);
}

TEST_F(chunk_test, test_stress)
{
    const int ITER_COUNT = 2000;
    std::stack<std::uint8_t*> st;
    for (int i = 0; i < ITER_COUNT; ++i) {
        if (rand() && chk.is_memory_available()) {
            std::uint8_t* ptr = chk.allocate(sizeof(int));
            ASSERT_NE(nullptr, ptr);
            // try dereference pointer
            // in case of incorrect allocation it might cause segmentation fault
            *ptr = 42;
            st.push(ptr);
        } else if (!st.empty()) {
            std::uint8_t* ptr = st.top();
            st.pop();
            ASSERT_TRUE(chk.is_owned(ptr, sizeof(int)));
            chk.deallocate(ptr, sizeof(int));
            ASSERT_TRUE(chk.is_memory_available());
        }
    }
}

//class memory_pool_test: public ::testing::Test
//{
//public:

//    typedef default_allocation_policy<int, allocation_traits<int>, statistic_policy<int>> alloc_policy;
//    typedef details::memory_pool<int, alloc_policy> pool_type;
//    typedef typename alloc_policy::statistic_type statistic;

//    static const int CHUNK_SIZE = pool_type::CHUNK_SIZE;

//    memory_pool_test():
//        pool(&alloc)
//      , rand(0.75)
//    {
//        alloc.set_statistic(&stat);
//    }

//    statistic stat;
//    alloc_policy alloc;
//    pool_type pool;
//    bernulli_generator rand;
//};

//TEST_F(memory_pool_test, test_allocate)
//{
//    int* ptr1 = pool.allocate();
//    EXPECT_NE(nullptr, ptr1);
//    // try dereference pointer
//    // in case of incorrect allocation it might cause segmentation fault
//    *ptr1 = 42;
//    EXPECT_EQ(1, stat.allocs_count());
//    EXPECT_EQ(1, stat.allocated_blocks_count());
//    EXPECT_EQ(CHUNK_SIZE * sizeof(int), stat.mem_used());

//    for (int i = 0; i < CHUNK_SIZE - 1; ++i) {
//        int* ptr2 = pool.allocate();
//        *ptr2 = 42;
//        EXPECT_EQ(1, stat.allocs_count());
//        EXPECT_EQ(1, stat.allocated_blocks_count());
//        EXPECT_EQ(CHUNK_SIZE * sizeof(int), stat.mem_used());
//    }

//    int* ptr3 = pool.allocate();
//    EXPECT_NE(nullptr, ptr3);
//    EXPECT_NE(ptr1, ptr3);
//    *ptr3 = 42;
//    EXPECT_EQ(2, stat.allocs_count());
//    EXPECT_EQ(2, stat.allocated_blocks_count());
//    EXPECT_EQ(2 * CHUNK_SIZE * sizeof(int), stat.mem_used());
//}

//TEST_F(memory_pool_test, test_deallocate)
//{
//    int* ptr1 = pool.allocate();
//    pool.deallocate(ptr1);
//    EXPECT_EQ(0, stat.deallocs_count());
//    EXPECT_EQ(1, stat.allocated_blocks_count());
//    EXPECT_EQ(CHUNK_SIZE * sizeof(int), stat.mem_used());
//}

//TEST_F(memory_pool_test, test_size)
//{
//    EXPECT_EQ(pool.size(), 0);
//    for (int i = 0; i < 4 * CHUNK_SIZE; ++i) {
//        pool.allocate();
//    }
//    pool.allocate();
//    EXPECT_EQ(5 * CHUNK_SIZE, pool.size());
//}

//TEST_F(memory_pool_test, test_reserve)
//{
//    pool.reserve(2 * CHUNK_SIZE);
//    EXPECT_EQ(2 * CHUNK_SIZE, pool.size());
//    for (int i = 0; i < 2 * CHUNK_SIZE; ++i) {
//        int* ptr = pool.allocate();
//        EXPECT_NE(nullptr, ptr);
//        *ptr = 42;
//    }

//    pool.reserve(CHUNK_SIZE);
//    EXPECT_EQ(2 * CHUNK_SIZE, pool.size());

//    int* ptr = pool.allocate();
//    *ptr = 42;
//    pool.reserve(5 * CHUNK_SIZE);
//    EXPECT_EQ(5 * CHUNK_SIZE, pool.size());
//    // check old memory is valid
//    EXPECT_EQ(42, *ptr);
//}

//TEST_F(memory_pool_test, test_stress)
//{
//    const int ITER_COUNT = 2000;
//    std::stack<int*> st;
//    for (int i = 0; i < ITER_COUNT; ++i) {
//        if (rand()) {
//            int* ptr = pool.allocate();
//            ASSERT_NE(nullptr, ptr);
//            // try dereference pointer
//            // in case of incorrect allocation it might cause segmentation fault
//            *ptr = 42;
//            st.push(ptr);
//        } else if (!st.empty()) {
//            int* ptr = st.top();
//            st.pop();
//            pool.deallocate(ptr);
//        }
//    }
//}

//class pool_allocation_policy_test: public ::testing::Test
//{
//public:

//    typedef pool_allocation_policy<int, allocation_traits<int>,
//                                        default_allocation_policy<int, allocation_traits<int>,
//                                            statistic_policy<int>
//                                        >
//                                  > allocator;
//    typedef details::memory_pool<int, allocator> pool_type;
//    typedef typename allocator::statistic_type statistic;

//    static const int CHUNK_SIZE = pool_type::CHUNK_SIZE;

//    pool_allocation_policy_test()
//    {
//        alloc.set_statistic(&stat);
//    }

//    statistic stat;
//    allocator alloc;
//};

//TEST_F(pool_allocation_policy_test, test_allocate)
//{
//    int* ptr1 = alloc.allocate(1, nullptr);
//    EXPECT_NE(nullptr, ptr1);
//    // try dereference pointer
//    // in case of incorrect allocation it might cause segmentation fault
//    *ptr1 = 42;
//    EXPECT_EQ(1, stat.allocs_count());
//    EXPECT_EQ(1, stat.allocated_blocks_count());
//    EXPECT_EQ(CHUNK_SIZE * sizeof(int), stat.mem_used());

//    int* ptr2 = alloc.allocate(2, nullptr);
//    EXPECT_NE(nullptr, ptr2);
//    ptr2[0] = ptr2[1] = 42;
//    EXPECT_EQ(2, stat.allocs_count());
//    EXPECT_EQ(2, stat.allocated_blocks_count());
//    EXPECT_LE((CHUNK_SIZE + 2) * sizeof(int), stat.mem_used());
//    alloc.deallocate(ptr2, 2);

//    int* ptr3 = alloc.allocate(1, nullptr);
//    int* ptr4 = alloc.allocate(1, ptr3);
//    EXPECT_EQ(ptr3, ptr4);
//}

//TEST_F(pool_allocation_policy_test, test_deallocate)
//{
//    int* ptr1 = alloc.allocate(1, nullptr);
//    alloc.deallocate(ptr1, 1);
//    EXPECT_EQ(0, stat.deallocs_count());
//    EXPECT_EQ(1, stat.allocated_blocks_count());
//    EXPECT_EQ(CHUNK_SIZE * sizeof(int), stat.mem_used());

//    int* ptr2 = alloc.allocate(2, nullptr);
//    alloc.deallocate(ptr2, 2);
//    EXPECT_EQ(1, stat.deallocs_count());
//    EXPECT_EQ(1, stat.allocated_blocks_count());
//    EXPECT_EQ(CHUNK_SIZE * sizeof(int), stat.mem_used());

//    alloc.deallocate(nullptr, 1);
//    EXPECT_EQ(1, stat.deallocs_count());
//    EXPECT_EQ(1, stat.allocated_blocks_count());
//    EXPECT_EQ(CHUNK_SIZE * sizeof(int), stat.mem_used());
//}
