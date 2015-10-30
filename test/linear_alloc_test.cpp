#include <cstdint>

#include <gtest/gtest.h>

#include "details/linear_storage.hpp"
#include "linear_allocation.hpp"
#include "statistic_policy.hpp"

using namespace alloc_utility;
using alloc_utility::details::linear_storage;

//class linear_storage_test: public ::testing::Test
//{
//public:
//    typedef std::uint8_t byte;
//    typedef linear_storage<byte*, size_t> storage_type;

//    static const size_t STORAGE_SIZE = 128;

//    linear_storage_test()
//    {
//        storage.set_storage(mem, STORAGE_SIZE);
//    }

//    byte mem[STORAGE_SIZE];
//    storage_type storage;
//};

//TEST_F(linear_storage_test, test_size)
//{
//    EXPECT_EQ((size_t)STORAGE_SIZE, storage.storage_size());
//}

//TEST_F(linear_storage_test, test_is_memory_available)
//{
//    EXPECT_TRUE(storage.is_memory_available(1));
//    EXPECT_TRUE(storage.is_memory_available(STORAGE_SIZE));
//    EXPECT_FALSE(storage.is_memory_available(STORAGE_SIZE + 1));

//    storage.allocate(STORAGE_SIZE);
//    EXPECT_FALSE(storage.is_memory_available(1));
//}

//TEST_F(linear_storage_test, test_is_owned)
//{
//    EXPECT_TRUE(storage.is_owned(mem));
//    EXPECT_FALSE(storage.is_owned(mem - 1));
//    EXPECT_FALSE(storage.is_owned(mem + STORAGE_SIZE));
//    EXPECT_FALSE(storage.is_owned(nullptr));
//}

//TEST_F(linear_storage_test, test_set_storage)
//{
//    byte new_mem[1];
//    storage.set_storage(new_mem, 1);
//    EXPECT_EQ(new_mem, storage.get_storage());
//    EXPECT_EQ(1, storage.storage_size());
//}

//TEST_F(linear_storage_test, test_allocate)
//{
//    byte* ptr1 = storage.allocate(1);
//    EXPECT_NE(nullptr, ptr1);
//    EXPECT_TRUE(storage.is_owned(ptr1));
//    // try dereference pointer
//    // in case of incorrect allocation it might cause segmentation fault
//    *ptr1 = 42;

//    byte* ptr2 = storage.allocate(1);
//    EXPECT_NE(nullptr, ptr2);
//    EXPECT_NE(ptr1, ptr2);
//    EXPECT_TRUE(storage.is_owned(ptr2));
//    *ptr2 = 42;
//}

//class linear_allocation_policy_test: public ::testing::Test
//{
//public:

//    typedef linear_allocation_policy<int, allocation_traits<int>,
//                                        default_allocation_policy<int, allocation_traits<int>,
//                                            statistic_policy<int>
//                                        >
//                                  > int_allocator;

//    typedef linear_allocation_policy<char, allocation_traits<char>,
//                                        default_allocation_policy<char, allocation_traits<char>,
//                                            statistic_policy<char>
//                                        >
//                                  > char_allocator;

//    typedef typename int_allocator::statistic_type statistic;
//    typedef std::uint8_t byte;

//    static const size_t STORAGE_SIZE = 128;

//    linear_allocation_policy_test()
//    {
//        alloc.set_storage((int*)mem, STORAGE_SIZE);
//        alloc.set_statistic(&stat);
//    }

//    statistic stat;
//    byte mem[STORAGE_SIZE * sizeof(int)];
//    int_allocator alloc;
//};

//TEST_F(linear_allocation_policy_test, test_is_memory_available)
//{
//    EXPECT_TRUE(alloc.is_memory_available(1));
//    EXPECT_TRUE(alloc.is_memory_available(STORAGE_SIZE));
//    EXPECT_FALSE(alloc.is_memory_available(STORAGE_SIZE + 1));

//    alloc.allocate(STORAGE_SIZE, nullptr);
//    EXPECT_FALSE(alloc.is_memory_available(1));
//}

//TEST_F(linear_allocation_policy_test, test_size)
//{
//    EXPECT_EQ((size_t)STORAGE_SIZE, alloc.storage_size());
//}

//TEST_F(linear_allocation_policy_test, test_set_storage)
//{
//    byte new_mem[1];
//    alloc.set_storage((int*)new_mem, 1);
//    EXPECT_EQ((int*)new_mem, alloc.get_storage());
//    EXPECT_EQ(1, alloc.storage_size());
//}

//TEST_F(linear_allocation_policy_test, test_allocate_storage)
//{
//    alloc.allocate_storage(2 * STORAGE_SIZE);
//    EXPECT_NE((int*)mem, alloc.get_storage());
//    EXPECT_EQ(2 * STORAGE_SIZE, alloc.storage_size());
//    EXPECT_EQ(1, stat.allocs_count());
//    EXPECT_EQ(1, stat.allocated_blocks_count());
//    EXPECT_EQ(2 * STORAGE_SIZE * sizeof(int), stat.mem_used());
//}

//TEST_F(linear_allocation_policy_test, test_allocate)
//{
//    int* ptr1 = alloc.allocate(STORAGE_SIZE, nullptr);
//    EXPECT_EQ((int*)mem, ptr1);
//    EXPECT_EQ(0, stat.allocs_count());
//    EXPECT_EQ(0, stat.allocated_blocks_count());
//    EXPECT_EQ(0, stat.mem_used());

//    int* ptr2 = alloc.allocate(1, nullptr);
//    EXPECT_NE(nullptr, ptr2);
//    EXPECT_NE(ptr1, ptr2);
//    // try dereference pointer
//    // in case of incorrect allocation it might cause segmentation fault
//    *ptr2 = 42;
//    EXPECT_EQ(1, stat.allocs_count());
//    EXPECT_EQ(1, stat.allocated_blocks_count());
//    EXPECT_EQ(sizeof(int), stat.mem_used());
//    alloc.deallocate(ptr2, 1);

//    int* ptr3 = alloc.allocate(1, nullptr);
//    int* ptr4 = alloc.allocate(1, ptr3);
//    EXPECT_EQ(ptr3, ptr4);
//}

//TEST_F(linear_allocation_policy_test, test_deallocate)
//{
//    int* ptr1 = alloc.allocate(STORAGE_SIZE, nullptr);
//    alloc.deallocate(ptr1, STORAGE_SIZE);
//    EXPECT_EQ(0, stat.deallocs_count());
//    EXPECT_EQ(0, stat.allocated_blocks_count());
//    EXPECT_EQ(0, stat.mem_used());

//    int* ptr2 = alloc.allocate(1, nullptr);
//    alloc.deallocate(ptr2, 1);
//    EXPECT_EQ(1, stat.deallocs_count());
//    EXPECT_EQ(0, stat.allocated_blocks_count());
//    EXPECT_EQ(0, stat.mem_used());
//}

//TEST_F(linear_allocation_policy_test, test_comparison)
//{
//    int_allocator other;
//    EXPECT_TRUE(alloc.operator==(alloc));
//    EXPECT_TRUE(alloc.operator!=(other));
//}

//TEST_F(linear_allocation_policy_test, test_copy_construct)
//{
//    int_allocator alloc_copy(alloc);
//    EXPECT_TRUE(alloc_copy.operator==(alloc));
//    EXPECT_TRUE(alloc.operator==(alloc_copy));
//    EXPECT_EQ(alloc.get_storage(), alloc_copy.get_storage());

//    char_allocator char_alloc(alloc);
//    EXPECT_TRUE(int_allocator(char_alloc).operator==(alloc));
//    EXPECT_TRUE(char_alloc.operator==(char_allocator(alloc)));
//    EXPECT_EQ(alloc.get_storage(), (int*)char_alloc.get_storage());

//    alloc.allocate(STORAGE_SIZE, nullptr);
//    EXPECT_FALSE(alloc_copy.is_memory_available(1));
//    EXPECT_FALSE(char_alloc.is_memory_available(1));
//}
