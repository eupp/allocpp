#ifndef POOL_ALLOCATION_HPP
#define POOL_ALLOCATION_HPP

#include <cstdint>
#include <cassert>
#include <limits>
#include <vector>
#include <memory>

#include "alloc_traits.hpp"
#include "alloc_policies.hpp"
#include "pointer_cast.hpp"
#include "macro.hpp"

namespace alloc_utility
{

namespace details
{

template <typename T, typename alloc_traits = allocation_traits<T>>
class chunk
{
public:

    DECLARE_ALLOC_TRAITS(T, alloc_traits)

    typedef rebind_pointer<std::uint8_t> raw_pointer;

    static const int CHUNK_SIZE = std::numeric_limits<std::uint8_t>::max();

    chunk()
    {
        set_pointer(pointer(nullptr));
    }

    explicit chunk(const pointer& chunk_ptr, bool delete_flag = true)
    {
        set_pointer(chunk_ptr, delete_flag);
    }

    chunk(chunk&& other) noexcept:
        m_chunk(other.m_chunk)
      , m_head(other.m_head)
      , m_available(other.m_available)
      , m_delete_flag(other.m_delete_flag)
    {}

    chunk(const chunk&) = delete;
    chunk& operator=(const chunk&) = delete;
    chunk& operator=(chunk&&) = delete;

    pointer get_pointer() const noexcept
    {
        return m_chunk;
    }

    void set_pointer(pointer ptr, bool delete_flag = true)
    {
        m_chunk = ptr;
        m_head = 0;
        m_available = CHUNK_SIZE;
        m_delete_flag = delete_flag;
        if (ptr) {
            for (int i = 1; i <= CHUNK_SIZE; ++i, ++ptr) {
                raw_pointer bytes = pointer_cast_traits<raw_pointer, pointer>::reinterpet_pointer_cast(ptr);
                bytes[0] = i;
            }
            // last block is initialized with CHUNK_SIZE
            // it is really doesn't matter because when we reach last block m_available == 0
        }
    }

    bool is_memory_available() const noexcept
    {
        return m_chunk && (m_available > 0);
    }

    bool is_owned(const pointer& ptr) const noexcept
    {
        return m_chunk && (m_chunk <= ptr) && (ptr < m_chunk + CHUNK_SIZE);
    }

    bool delete_flag() const noexcept
    {
        return m_delete_flag;
    }

    pointer allocate()
    {
        if (is_memory_available()) {
            pointer ptr = m_chunk + m_head;
            raw_pointer bytes = pointer_cast_traits<raw_pointer, pointer>::reinterpet_pointer_cast(ptr);
            m_head = bytes[0];
            m_available--;
            return ptr;
        }
        return pointer(nullptr);
    }

    void deallocate(const pointer& ptr)
    {
        difference_type diff = ptr - m_chunk;
        raw_pointer byte = pointer_cast_traits<raw_pointer, pointer>::reinterpet_pointer_cast(ptr);
        byte[0] = m_head;
        m_head = diff;
        m_available++;
    }

private:

    pointer m_chunk;
    std::uint8_t m_head;
    std::uint8_t m_available;
    bool m_delete_flag;
};

template <typename T, typename allocator, typename alloc_traits = allocation_traits<T>>
class memory_pool
{
public:

    DECLARE_ALLOC_TRAITS(T, alloc_traits)

    static const int CHUNK_SIZE = chunk<T, alloc_traits>::CHUNK_SIZE;

    memory_pool(allocator* alloc) noexcept:
        m_alloc(alloc)
    {
        assert(alloc);
    }

    ~memory_pool()
    {
        for (auto& chunk: m_pool) {
            if (chunk.delete_flag()) {
                m_alloc->deallocate(chunk.get_pointer(), CHUNK_SIZE);
            }
        }
    }

    memory_pool(const memory_pool&) = delete;
    memory_pool(memory_pool&&) = delete;
    memory_pool& operator=(const memory_pool&) = delete;
    memory_pool& operator=(memory_pool&&) = delete;

    size_type size() const noexcept
    {
        return CHUNK_SIZE * m_pool.size();
    }

    void reserve(size_type new_size)
    {
        if (new_size <= size()) {
            return;
        }
        int old_chunks_count = m_pool.size();
        int chunks_count = (new_size + CHUNK_SIZE - 1) / CHUNK_SIZE;
        int chunks_diff = chunks_count - old_chunks_count;
        m_pool.resize(chunks_count);
        pointer ptr = m_alloc->allocate(chunks_diff * CHUNK_SIZE, pointer(nullptr));

        int i = old_chunks_count;
        m_pool[i].set_pointer(ptr, true);
        ++i;
        ptr += CHUNK_SIZE;
        for (; i < chunks_count; ++i, ptr += CHUNK_SIZE) {
            m_pool[i].set_pointer(ptr, false);
        }
    }

    pointer allocate(const const_void_pointer& hint = nullptr)
    {
        ALLOC_UNUSED(hint);
        for (auto& chunk: m_pool) {
            if (chunk.is_memory_available()) {
                return chunk.allocate();
            }
        }
        m_pool.emplace_back(m_alloc->allocate(CHUNK_SIZE, pointer(nullptr)));
        return m_pool.back().allocate();
    }

    void deallocate(const pointer& ptr)
    {
        for (auto& chunk: m_pool) {
            if (chunk.is_owned(ptr)) {
                chunk.deallocate(ptr);
            }
        }
    }

private:
    std::vector<details::chunk<T, alloc_traits>> m_pool;
    allocator* m_alloc;
};

} // namespace details

template <typename T, typename alloc_traits = allocation_traits<T>,
          typename base_policy = default_allocation_policy<T, alloc_traits>>
class pool_allocation_policy: public base_policy
{
    typedef details::memory_pool<T, base_policy, alloc_traits> pool_type;

public:

    DECLARE_ALLOC_TRAITS(T, alloc_traits)
    DECLARE_REBIND_ALLOC(pool_allocation_policy, T, alloc_traits, base_policy)

    explicit pool_allocation_policy():
        m_pool(std::make_shared<pool_type>(this))
    {}

    pool_allocation_policy(const pool_allocation_policy& other):
        base_policy(other)
      , m_pool(other.m_pool)
    {}

    template <typename U>
    pool_allocation_policy(const pool_allocation_policy::rebind<U>& other):
        base_policy(other)
      , m_pool(std::make_shared<pool_type>(this))
    {}

    pool_allocation_policy& operator=(const pool_allocation_policy&) = delete;
    pool_allocation_policy& operator=(pool_allocation_policy&&) = delete;

    size_type size() const noexcept
    {
        return m_pool->size();
    }

    void reserve(size_type new_size)
    {
        m_pool->reserve(new_size);
    }

    pointer allocate(size_type n, const pointer& ptr, const const_void_pointer& hint = nullptr)
    {
        if (ptr) {
            return ptr;
        }
        if (n > 1) {
            return base_policy::allocate(n, ptr, hint);
        }
        return m_pool->allocate(hint);
    }

    void deallocate(const pointer& ptr, size_type n)
    {
        if (n > 1) {
            base_policy::deallocate(ptr, n);
        }
        m_pool->deallocate(ptr);
    }

private:
    std::shared_ptr<pool_type> m_pool;
};

} // namespace alloc_utility

#endif // POOL_ALLOCATION_HPP
