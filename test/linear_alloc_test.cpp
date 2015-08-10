#include <cstdint>

#include <gtest/gtest.h>

#include "details/linear_storage.hpp"
#include "linear_allocation.hpp"

using namespace alloc_utility;
using alloc_utility::details::linear_storage;

class linear_storage_test: public ::testing::Test
{
public:
    typedef std::uint8_t byte;
    typedef linear_storage<byte*, size_t> storage_type;

    static const size_t STORAGE_SIZE = 128;

    linear_storage_test()
    {
        storage.set_storage(mem, STORAGE_SIZE);
    }

    byte mem[STORAGE_SIZE];
    storage_type storage;
};

TEST_F(linear_storage_test, test_size)
{
    EXPECT_EQ((size_t)STORAGE_SIZE, storage.storage_size());
}

TEST_F(linear_storage_test, test_is_memory_available)
{
    EXPECT_TRUE(storage.is_memory_available(1));
    EXPECT_TRUE(storage.is_memory_available(STORAGE_SIZE));
    EXPECT_FALSE(storage.is_memory_available(STORAGE_SIZE + 1));

    storage.allocate(STORAGE_SIZE);
    EXPECT_FALSE(storage.is_memory_available(1));
}

TEST_F(linear_storage_test, test_is_owned)
{
    EXPECT_TRUE(storage.is_owned(mem));
    EXPECT_FALSE(storage.is_owned(mem - 1));
    EXPECT_FALSE(storage.is_owned(mem + STORAGE_SIZE));
    EXPECT_FALSE(storage.is_owned(nullptr));
}

TEST_F(linear_storage_test, test_set_storage)
{
    byte new_mem[1];
    storage.set_storage(new_mem, 1);
    EXPECT_EQ(new_mem, storage.get_storage());
    EXPECT_EQ(1, storage.storage_size());
}

TEST_F(linear_storage_test, test_allocate)
{
    byte* ptr1 = storage.allocate(1);
    EXPECT_NE(nullptr, ptr1);
    EXPECT_TRUE(storage.is_owned(ptr1));
    // try dereference pointer
    // in case of incorrect allocation it might cause segmentation fault
    *ptr1 = 42;

    byte* ptr2 = storage.allocate(1);
    EXPECT_NE(nullptr, ptr2);
    EXPECT_NE(ptr1, ptr2);
    EXPECT_TRUE(storage.is_owned(ptr2));
    *ptr2 = 42;
}
