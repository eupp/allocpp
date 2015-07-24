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

    template <typename U>
    using rebind_pointer = typename alloc_traits::template rebind_pointer<U>;

    typedef rebind_pointer<std::uint8_t> raw_pointer;

    static const int CHUNK_SIZE = std::numeric_limits<std::uint8_t>::max();

    chunk()
    {
        set_pointer(pointer(nullptr));
    }

    explicit chunk(const pointer& chunk_ptr)
    {
        set_pointer(chunk_ptr);
    }

    chunk(chunk&& other) noexcept:
        m_chunk(other.m_chunk)
      , m_head(other.m_head)
      , m_available(other.m_available)
    {}

    chunk(const chunk&) = delete;
    chunk& operator=(const chunk&) = delete;
    chunk& operator=(chunk&&) = delete;

    pointer get_pointer() const
    {
        return m_chunk;
    }

    void set_pointer(pointer ptr)
    {
        m_chunk = ptr;
        m_head = 0;
        m_available = CHUNK_SIZE;
        if (ptr) {
            for (int i = 1; i <= CHUNK_SIZE; ++i, ++ptr) {
                raw_pointer bytes = pointer_cast_traits<raw_pointer, pointer>::reinterpet_pointer_cast(ptr);
                bytes[0] = i;
            }
            // last block is initialized with CHUNK_SIZE
            // it is really doesn't matter because when we reach last block m_available == 0
        }
    }

    bool is_memory_available() const
    {
        return m_chunk && (m_available > 0);
    }

    bool is_owned(const pointer& ptr) const
    {
        return m_chunk && (m_chunk <= ptr) && (ptr < m_chunk + CHUNK_SIZE);
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
};

template <typename T, typename allocator, typename alloc_traits = allocation_traits<T>>
class memory_pool
{
public:

    DECLARE_ALLOC_TRAITS(T, alloc_traits)

    static const int CHUNK_SIZE = chunk<T, alloc_traits>::CHUNK_SIZE;

    memory_pool(allocator* alloc):
        // calculate required number of chunks
        m_alloc(alloc)
    {
        assert(alloc);
    }

    ~memory_pool()
    {
        for (auto& chunk: m_pool) {
            m_alloc->deallocate(chunk.get_pointer(), CHUNK_SIZE);
        }
    }

    memory_pool(const memory_pool&) = delete;
    memory_pool(memory_pool&&) = delete;
    memory_pool& operator=(const memory_pool&) = delete;
    memory_pool& operator=(memory_pool&&) = delete;

    void reserve(size_type size)
    {
        size_type curr_size = CHUNK_SIZE * m_pool.size();
        if (size <= curr_size) {
            return;
        }
        int old_chunks_count = m_pool.size();
        int chunks_count = (size + CHUNK_SIZE - 1) / CHUNK_SIZE;
        m_pool.resize(chunks_count);
        for (int i = old_chunks_count; i < chunks_count; ++i) {
            m_pool[i].set_pointer(m_alloc->allocate(CHUNK_SIZE, pointer(nullptr)));
        }
    }

    pointer allocate(size_type n, const pointer& ptr, const_void_pointer hint = nullptr)
    {
        if (ptr) {
            return ptr;
        }
        if (n > 1) {
            return m_alloc->allocate(n, ptr, hint);
        }
        for (auto& chunk: m_pool) {
            if (chunk.is_memory_available()) {
                return chunk.allocate();
            }
        }
        m_pool.emplace_back(m_alloc->allocate(CHUNK_SIZE, pointer(nullptr)));
        return m_pool.back().allocate();
    }

    void deallocate(const pointer& ptr, size_type n)
    {
        if (!ptr) {
            return;
        }
        if (n > 1) {
            m_alloc->deallocate(ptr, n);
        }
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

template <typename T, typename alloc_traits = allocation_traits<T>, typename base_policy = default_allocation_policy<T>>
class pool_allocation_policy: public base_policy
{
    typedef details::memory_pool<T, base_policy, alloc_traits> pool_type;

public:

    DECLARE_ALLOC_POLICY_WT(pool_allocation_policy, base_policy, T, alloc_traits)

    static const int CHUNK_SIZE = details::memory_pool<T, alloc_traits>::CHUNK_SIZE;

    explicit pool_allocation_policy():
        // calculate required number of chunks
        m_pool(std::make_shared<pool_type>(this))
    {}

    template <typename U>
    pool_allocation_policy(const pool_allocation_policy::rebind<U>& other):
        pool_allocation_policy(pool_allocation_policy())
    {}

    pool_allocation_policy& operator=(const pool_allocation_policy&) = delete;
    pool_allocation_policy& operator=(pool_allocation_policy&&) = delete;


    pointer allocate(size_type n, const pointer& ptr, const_void_pointer hint = nullptr)
    {
        return m_pool->allocate(n, ptr, hint);
    }

    void deallocate(const pointer& ptr, size_type n)
    {
        m_pool->deallocate(ptr, n);
    }

private:
    std::shared_ptr<pool_type> m_pool;
};

} // namespace alloc_utility

#endif // POOL_ALLOCATION_HPP
