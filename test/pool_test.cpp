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
using alloc_utility::details::memory_block;

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

    typedef std::uint8_t byte;

    static const int CHUNK_SIZE = chunk<byte*, size_t>::CHUNK_MAXSIZE;
    static const int OBJ_SIZE = sizeof(int);

    chunk_test():
        mem(new byte[OBJ_SIZE * CHUNK_SIZE])
      , chk(mem, OBJ_SIZE)
      , rand(0.75)
    {}

    ~chunk_test()
    {
        delete[] mem;
    }

    byte* mem;
    chunk<byte*, size_t> chk;
    bernulli_generator rand;
};

TEST_F(chunk_test, test_size)
{
    EXPECT_EQ((size_t)CHUNK_SIZE, chk.size());
}

TEST_F(chunk_test, test_is_memory_available)
{
    EXPECT_TRUE(chk.is_memory_available());

    for (int i = 0; i < CHUNK_SIZE; ++i) {
        chk.allocate(OBJ_SIZE);
    }
    EXPECT_FALSE(chk.is_memory_available());
}

TEST_F(chunk_test, test_is_owned)
{
    EXPECT_TRUE(chk.is_owned(mem, OBJ_SIZE));
    EXPECT_FALSE(chk.is_owned(mem + (CHUNK_SIZE + 10) * OBJ_SIZE, OBJ_SIZE));
    EXPECT_FALSE(chk.is_owned(mem - OBJ_SIZE, OBJ_SIZE));
    EXPECT_FALSE(chk.is_owned(nullptr, OBJ_SIZE));
}

TEST_F(chunk_test, test_allocate)
{
    byte* ptr1 = chk.allocate(OBJ_SIZE);
    EXPECT_NE(nullptr, ptr1);
    EXPECT_TRUE(chk.is_owned(ptr1, OBJ_SIZE));
    // try dereference pointer
    // in case of incorrect allocation it might cause segmentation fault
    *ptr1 = 42;

    byte* ptr2 = chk.allocate(OBJ_SIZE);
    EXPECT_NE(nullptr, ptr2);
    EXPECT_NE(ptr1, ptr2);
    EXPECT_TRUE(chk.is_owned(ptr2, OBJ_SIZE));
    *ptr2 = 42;

    for (int i = 0; i < CHUNK_SIZE - 2; ++i) {
        byte* ptr3 = chk.allocate(OBJ_SIZE);
        EXPECT_TRUE(chk.is_owned(ptr3, OBJ_SIZE));
        *ptr3 = 42;
    }

    byte* ptr4 = chk.allocate(OBJ_SIZE);
    EXPECT_EQ(nullptr, ptr4);
}

TEST_F(chunk_test, test_deallocate)
{
    byte* ptr = nullptr;
    for (int i = 0; i < CHUNK_SIZE; ++i) {
        ptr = chk.allocate(OBJ_SIZE);
    }
    chk.deallocate(ptr, OBJ_SIZE);
    EXPECT_TRUE(chk.is_memory_available());

    ptr = nullptr;
    ptr = chk.allocate(OBJ_SIZE);
    EXPECT_NE(nullptr, ptr);
}

TEST_F(chunk_test, test_stress)
{
    const int ITER_COUNT = 2000;
    std::stack<byte*> st;
    for (int i = 0; i < ITER_COUNT; ++i) {
        if (rand() && chk.is_memory_available()) {
            byte* ptr = chk.allocate(OBJ_SIZE);
            ASSERT_NE(nullptr, ptr);
            // try dereference pointer
            // in case of incorrect allocation it might cause segmentation fault
            *ptr = 42;
            st.push(ptr);
        } else if (!st.empty()) {
            byte* ptr = st.top();
            st.pop();
            ASSERT_TRUE(chk.is_owned(ptr, OBJ_SIZE));
            chk.deallocate(ptr, OBJ_SIZE);
            ASSERT_TRUE(chk.is_memory_available());
        }
    }
}

class memory_block_test: public ::testing::Test
{
public:

    typedef std::uint8_t byte;
    typedef typename memory_block<byte*, size_t>::chunk_it chunk_it;

    static const int BLOCK_SIZE = 4 * chunk<byte*, size_t>::CHUNK_MAXSIZE + 1;
    static const int OBJ_SIZE = sizeof(int);

    memory_block_test():
        mem(new byte[OBJ_SIZE * BLOCK_SIZE])
      , block(mem, OBJ_SIZE, BLOCK_SIZE)
      , rand(0.75)
    {}

    ~memory_block_test()
    {
        delete[] mem;
    }

    byte* mem;
    memory_block<byte*, size_t> block;
    bernulli_generator rand;
};

TEST_F(memory_block_test, test_size)
{
    EXPECT_EQ((size_t)BLOCK_SIZE, block.size());
}

TEST_F(memory_block_test, test_is_memory_available)
{
    EXPECT_TRUE(block.is_memory_available());

    for (int i = 0; i < BLOCK_SIZE; ++i) {
        block.allocate(OBJ_SIZE);
    }
    EXPECT_FALSE(block.is_memory_available());
}

TEST_F(memory_block_test, test_is_owned)
{
    EXPECT_TRUE(block.is_owned(mem, OBJ_SIZE));
    EXPECT_FALSE(block.is_owned(mem + (BLOCK_SIZE + 10) * OBJ_SIZE, OBJ_SIZE));
    EXPECT_FALSE(block.is_owned(mem - OBJ_SIZE, OBJ_SIZE));
    EXPECT_FALSE(block.is_owned(nullptr, OBJ_SIZE));
}

TEST_F(memory_block_test, test_allocate)
{
    auto res = block.allocate(OBJ_SIZE);
    byte* ptr1 = res.first;
    chunk_it chk = res.second;
    EXPECT_NE(nullptr, ptr1);
    EXPECT_TRUE(block.is_owned(ptr1, OBJ_SIZE));
    EXPECT_TRUE(chk->is_owned(ptr1, OBJ_SIZE));
    // try dereference pointer
    // in case of incorrect allocation it might cause segmentation fault
    *ptr1 = 42;

    byte* ptr2 = block.allocate(OBJ_SIZE).first;
    EXPECT_NE(nullptr, ptr2);
    EXPECT_NE(ptr1, ptr2);
    EXPECT_TRUE(block.is_owned(ptr2, OBJ_SIZE));
    *ptr2 = 42;

    for (int i = 0; i < BLOCK_SIZE - 2; ++i) {
        byte* ptr3 = block.allocate(OBJ_SIZE).first;
        EXPECT_TRUE(block.is_owned(ptr3, OBJ_SIZE));
        *ptr3 = 42;
    }

    byte* ptr4 = block.allocate(OBJ_SIZE).first;
    EXPECT_EQ(nullptr, ptr4);
}

TEST_F(memory_block_test, test_deallocate)
{
    byte* ptr = nullptr;
    for (int i = 0; i < BLOCK_SIZE; ++i) {
        ptr = block.allocate(OBJ_SIZE).first;
    }
    block.deallocate(ptr, OBJ_SIZE);
    EXPECT_TRUE(block.is_memory_available());

    ptr = nullptr;
    ptr = block.allocate(OBJ_SIZE).first;
    EXPECT_NE(nullptr, ptr);
}

TEST_F(memory_block_test, test_stress)
{
    const int ITER_COUNT = 2000;
    std::stack<byte*> st;
    for (int i = 0; i < ITER_COUNT; ++i) {
        if (rand() && block.is_memory_available()) {
            byte* ptr = block.allocate(OBJ_SIZE).first;
            ASSERT_NE(nullptr, ptr);
            // try dereference pointer
            // in case of incorrect allocation it might cause segmentation fault
            *ptr = 42;
            st.push(ptr);
        } else if (!st.empty()) {
            byte* ptr = st.top();
            st.pop();
            ASSERT_TRUE(block.is_owned(ptr, OBJ_SIZE));
            block.deallocate(ptr, OBJ_SIZE);
            ASSERT_TRUE(block.is_memory_available());
        }
    }
}

class memory_pool_test: public ::testing::Test
{
public:

    typedef std::uint8_t byte;

    static const int BLOCK_SIZE = 4 * chunk<byte*, size_t>::CHUNK_MAXSIZE + 1;
    static const int OBJ_SIZE = sizeof(int);
    static const int OBJ_NUM = 10;

    typedef details::memory_pool<byte*, size_t> pool_type;
    typedef pool_type::memory_blocks_range mb_range_type;

    memory_pool_test():
        pool(OBJ_SIZE)
      , mem1(new byte[OBJ_NUM * OBJ_SIZE])
      , mem2(new byte[OBJ_NUM * OBJ_SIZE])
    {}

    ~memory_pool_test()
    {
        delete[] mem1;
        delete[] mem2;
    }

    pool_type pool;
    byte* mem1;
    byte* mem2;
};

TEST_F(memory_pool_test, test_obj_size)
{
    EXPECT_EQ((size_t)OBJ_SIZE, pool.obj_size());
}

TEST_F(memory_pool_test, test_add_mem_block)
{
    EXPECT_EQ(0, pool.size());

    pool.add_mem_block(mem1, OBJ_NUM);
    EXPECT_EQ((size_t)OBJ_NUM, pool.size());

    byte* mem2 = new byte[OBJ_NUM * OBJ_SIZE];
    pool.add_mem_block(mem2, OBJ_NUM);
    EXPECT_EQ(2 * OBJ_NUM, pool.size());
}

TEST_F(memory_pool_test, test_is_memory_available)
{
    pool.add_mem_block(mem1, OBJ_NUM);
    EXPECT_TRUE(pool.is_memory_available());

    for (int i = 0; i < OBJ_NUM; ++i) {
        pool.allocate();
    }
    EXPECT_FALSE(pool.is_memory_available());

    pool.add_mem_block(mem2, OBJ_NUM);
    EXPECT_TRUE(pool.is_memory_available());
}

TEST_F(memory_pool_test, test_is_owned)
{
    pool.add_mem_block(mem1, OBJ_NUM);
    EXPECT_TRUE(pool.is_owned(mem1));
    EXPECT_FALSE(pool.is_owned(mem1 + OBJ_SIZE * (OBJ_NUM + 10)));
    EXPECT_FALSE(pool.is_owned(mem1 - OBJ_SIZE));
    EXPECT_FALSE(pool.is_owned(nullptr));
    pool.add_mem_block(mem2, OBJ_NUM);
    EXPECT_TRUE(pool.is_owned(mem1));
    EXPECT_TRUE(pool.is_owned(mem2));
}

TEST_F(memory_pool_test, test_get_mem_blocks)
{
    pool.add_mem_block(mem1, OBJ_NUM);
    pool.add_mem_block(mem2, OBJ_NUM);

    mb_range_type blocks = pool.get_mem_blocks();
    EXPECT_FALSE(blocks.is_empty());
    EXPECT_EQ(2, blocks.count());
    for (auto it = blocks.begin(); it != blocks.end(); ++it) {
        byte* ptr = it->get_memory_ptr();
        EXPECT_TRUE(ptr == mem1 || ptr == mem2);
        EXPECT_EQ((size_t)OBJ_NUM, it->size());
    }
}

TEST_F(memory_pool_test, test_allocate)
{
    pool.add_mem_block(mem1, OBJ_NUM);
    pool.add_mem_block(mem2, OBJ_NUM);

    byte* ptr1 = pool.allocate();
    EXPECT_NE(nullptr, ptr1);
    EXPECT_TRUE(pool.is_owned(ptr1));
    // try dereference pointer
    // in case of incorrect allocation it might cause segmentation fault
    *ptr1 = 42;

    byte* ptr2 = pool.allocate();
    EXPECT_NE(nullptr, ptr2);
    EXPECT_NE(ptr1, ptr2);
    EXPECT_TRUE(pool.is_owned(ptr2));
    *ptr2 = 42;

    for (int i = 0; i < pool.size() - 2; ++i) {
        byte* ptr3 = pool.allocate();
        EXPECT_TRUE(pool.is_owned(ptr3));
        *ptr3 = 42;
    }

    byte* ptr4 = pool.allocate();
    EXPECT_EQ(nullptr, ptr4);
}

TEST_F(memory_pool_test, test_deallocate)
{
    pool.add_mem_block(mem1, OBJ_NUM);
    byte* ptr = nullptr;
    for (int i = 0; i < pool.size(); ++i) {
        ptr = pool.allocate();
    }
    pool.deallocate(ptr);
    EXPECT_TRUE(pool.is_memory_available());

    ptr = nullptr;
    ptr = pool.allocate();
    EXPECT_NE(nullptr, ptr);
}

class pool_allocation_policy_test: public ::testing::Test
{
public:

    typedef pool_allocation_policy<int, allocation_traits<int>,
                                        default_allocation_policy<int, allocation_traits<int>,
                                            statistic_policy<int>
                                        >
                                  > int_allocator;

    typedef pool_allocation_policy<char, allocation_traits<char>,
                                        default_allocation_policy<char, allocation_traits<char>,
                                            statistic_policy<char>
                                        >
                                  > char_allocator;

    typedef typename int_allocator::statistic_type statistic;

    pool_allocation_policy_test()
    {
        alloc.set_statistic(&stat);
    }

    statistic stat;
    int_allocator alloc;
};

TEST_F(pool_allocation_policy_test, test_allocate)
{
    int* ptr1 = alloc.allocate(1, nullptr);
    EXPECT_NE(nullptr, ptr1);
    // try dereference pointer
    // in case of incorrect allocation it might cause segmentation fault
    *ptr1 = 42;
    EXPECT_EQ(1, stat.allocs_count());
    EXPECT_EQ(1, stat.allocated_blocks_count());
    EXPECT_EQ(alloc.block_size() * sizeof(int), stat.mem_used());

    for (int i = 0; i < alloc.block_size() - 1; ++i) {
        int* ptr2 = alloc.allocate(1, nullptr);
        *ptr2 = 42;
        EXPECT_EQ(1, stat.allocs_count());
        EXPECT_EQ(1, stat.allocated_blocks_count());
        EXPECT_EQ(alloc.block_size() * sizeof(int), stat.mem_used());
    }

    int* ptr3 = alloc.allocate(1, nullptr);
    EXPECT_NE(nullptr, ptr3);
    EXPECT_NE(ptr1, ptr3);
    *ptr3 = 42;
    EXPECT_EQ(2, stat.allocs_count());
    EXPECT_EQ(2, stat.allocated_blocks_count());
    EXPECT_EQ(2 * alloc.block_size() * sizeof(int), stat.mem_used());

    int* ptr4 = alloc.allocate(2, nullptr);
    EXPECT_NE(nullptr, ptr4);
    ptr4[0] = ptr4[1] = 42;
    EXPECT_EQ(3, stat.allocs_count());
    EXPECT_EQ(3, stat.allocated_blocks_count());
    EXPECT_LE((2 * alloc.block_size() + 2) * sizeof(int), stat.mem_used());
    alloc.deallocate(ptr4, 2);

    int* ptr5 = alloc.allocate(1, nullptr);
    int* ptr6 = alloc.allocate(1, ptr5);
    EXPECT_EQ(ptr5, ptr6);
}

TEST_F(pool_allocation_policy_test, test_deallocate)
{
    int* ptr1 = alloc.allocate(1, nullptr);
    alloc.deallocate(ptr1, 1);
    EXPECT_EQ(0, stat.deallocs_count());
    EXPECT_EQ(1, stat.allocated_blocks_count());
    EXPECT_EQ(alloc.block_size() * sizeof(int), stat.mem_used());

    int* ptr2 = alloc.allocate(2, nullptr);
    alloc.deallocate(ptr2, 2);
    EXPECT_EQ(1, stat.deallocs_count());
    EXPECT_EQ(1, stat.allocated_blocks_count());
    EXPECT_EQ(alloc.block_size() * sizeof(int), stat.mem_used());

    alloc.deallocate(nullptr, 1);
    EXPECT_EQ(1, stat.deallocs_count());
    EXPECT_EQ(1, stat.allocated_blocks_count());
    EXPECT_EQ(alloc.block_size() * sizeof(int), stat.mem_used());
}

TEST_F(pool_allocation_policy_test, test_block_size)
{
    EXPECT_EQ((size_t)int_allocator::DEFAULT_BLOCK_SIZE, alloc.block_size());
    alloc.set_block_size(1);
    EXPECT_EQ(1, alloc.block_size());
}

TEST_F(pool_allocation_policy_test, test_capacity)
{
    EXPECT_EQ(alloc.capacity(), 0);
    for (int i = 0; i < 4 * alloc.block_size(); ++i) {
        alloc.allocate(1, nullptr);
    }
    alloc.allocate(1, nullptr);
    EXPECT_EQ(5 * alloc.block_size(), alloc.capacity());
}

TEST_F(pool_allocation_policy_test, test_reserve)
{
    alloc.reserve(2 * alloc.block_size());
    EXPECT_EQ(2 * alloc.block_size(), alloc.capacity());
    for (int i = 0; i < 2 * alloc.block_size(); ++i) {
        int* ptr = alloc.allocate(1, nullptr);
        EXPECT_NE(nullptr, ptr);
        *ptr = 42;
    }

    alloc.reserve(alloc.block_size());
    EXPECT_EQ(2 * alloc.block_size(), alloc.capacity());

    int* ptr = alloc.allocate(1, nullptr);
    *ptr = 42;
    alloc.reserve(5 * alloc.block_size());
    EXPECT_EQ(5 * alloc.block_size(), alloc.capacity());
    // check old memory is valid
    EXPECT_EQ(42, *ptr);
}

TEST_F(pool_allocation_policy_test, test_construct)
{
    int_allocator alloc_copy(alloc);
    EXPECT_TRUE(alloc_copy.operator==(alloc));
    EXPECT_TRUE(alloc.operator==(alloc_copy));
    int* ptr = alloc_copy.allocate(1, nullptr);
    EXPECT_EQ(1, stat.allocs_count());
    EXPECT_EQ(1, stat.allocated_blocks_count());
    EXPECT_EQ(alloc.block_size() * sizeof(int), stat.mem_used());

    EXPECT_TRUE(alloc.operator==(alloc_copy));
    alloc.deallocate(ptr, 1);
    EXPECT_EQ(0, stat.deallocs_count());
    EXPECT_EQ(1, stat.allocated_blocks_count());
    EXPECT_EQ(alloc.block_size() * sizeof(int), stat.mem_used());

    char_allocator char_alloc(alloc);
    EXPECT_TRUE(int_allocator(char_alloc).operator==(alloc));
    EXPECT_TRUE(char_alloc.operator==(char_allocator(alloc)));
    char* char_ptr = char_alloc.allocate(1, nullptr);
    EXPECT_EQ(2, stat.allocs_count());
    EXPECT_EQ(2, stat.allocated_blocks_count());
    EXPECT_EQ(alloc.block_size() * (sizeof(int) + sizeof(char)), stat.mem_used());

    EXPECT_TRUE(int_allocator(char_alloc).operator==(alloc));
    EXPECT_TRUE(char_alloc.operator==(char_allocator(alloc)));
    alloc.deallocate(reinterpret_cast<int*>(char_ptr), 1);
    EXPECT_EQ(0, stat.deallocs_count());
    EXPECT_EQ(2, stat.allocated_blocks_count());
    EXPECT_EQ(alloc.block_size() * (sizeof(int) + sizeof(char)), stat.mem_used());
}

TEST_F(pool_allocation_policy_test, test_destruct)
{
    alloc.reserve(100);
    alloc.~pool_allocation_policy();
    EXPECT_EQ(1, stat.deallocs_count());
    EXPECT_EQ(0, stat.allocated_blocks_count());
    EXPECT_EQ(0, stat.mem_used());
}
