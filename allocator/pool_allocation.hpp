#ifndef POOL_ALLOCATION_HPP
#define POOL_ALLOCATION_HPP

#include <cstdint>
#include <vector>

#include "alloc_traits.hpp"
#include "alloc_policies.hpp"
#include "pointer_cast.hpp"

namespace alloc_utility
{

namespace details
{

template <typename T, typename alloc_traits = allocation_traits<T>>
class chunk
{
public:

    typedef T value_type;
    typedef typename alloc_traits::pointer pointer;
    typedef typename alloc_traits::const_pointer const_pointer;
    typedef typename alloc_traits::size_type size_type;
    typedef typename alloc_traits::difference_type difference_type;

    typedef alloc_traits allocation_traits;

    typedef rebind_pointer<uint8_t> raw_pointer;

    static const int CHUNK_SIZE = std::UINT8_MAX;

    chunk()
    {
        set_pointer(pointer(nullptr));
    }

    chunk(const pointer& chunk_ptr)
    {
        set_pointer(chunk_ptr);
    }

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
        return (m_chunk <= ptr) && (ptr < m_chunk + CHUNK_SIZE);
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
        raw_pointer head = pointer_cast_traits<raw_pointer, pointer>::reinterpet_pointer_cast(m_chunk + m_head);
        raw_pointer inserted = pointer_cast_traits<raw_pointer, pointer>::reinterpet_pointer_cast(ptr);
        inserted[0] = head[0];
        head[0] = diff;
        m_available++;
    }

private:

    pointer m_chunk;
    std::uint8_t m_head;
    std::uint8_t m_available;
};

} // namespace details

template <typename T, typename alloc_traits = allocation_traits<T>, typename base_policy = default_allocation_policy<T>>
class pool_allocation_policy: public base_policy
{
public:

    typedef T value_type;
    typedef typename alloc_traits::pointer pointer;
    typedef typename alloc_traits::const_pointer const_pointer;
    typedef typename alloc_traits::size_type size_type;

    typedef alloc_traits allocation_traits;

    template <typename U>
    using rebind = pool_allocation_policy<U>;

    template <typename policy>
    using rebind_base = pool_allocation_policy<
                                                T,
                                                alloc_traits,
                                                typename policy::template rebind<T>
                                              >;

    static const int CHUNK_SIZE = details::chunk::CHUNK_SIZE;

    pool_allocation_policy(size_type n = CHUNK_SIZE):
        // calculate required number of chunks
        m_pool((n + CHUNK_SIZE - 1) / CHUNK_SIZE)
    {
        for (auto& chunk: m_pool) {
            chunk.set_pointer(base_policy::allocate(CHUNK_SIZE));
        }
    }

    ~pool_allocation_policy()
    {
        for (auto& chunk: m_pool) {
            base_policy::deallocate(chunk.get_pointer(), CHUNK_SIZE);
        }
    }

    pointer allocate(size_type n, const pointer& ptr, std::allocator<void>::const_pointer hint = nullptr)
    {
        if (ptr) {
            return ptr;
        }
        if (n > 1) {
            return base_policy::allocate(n, ptr, hint);
        }
        for (auto& chunk: m_pool) {
            if (chunk.is_memory_available()) {
                return chunk.allocate();
            }
        }
        m_pool.emplace_back(base_policy::allocate(CHUNK_SIZE));
        return m_pool.back().allocate();
    }

    void deallocate(const pointer& ptr, size_type n)
    {
        if (!ptr) {
            return;
        }
        if (n > 1) {
            base_policy::deallocate(ptr, n);
        }
        for (auto& chunk: m_pool) {
            if (chunk.is_owned(ptr)) {
                chunk.deallocate(ptr);
            }
        }
    }

private:
    std::vector<details::chunk<T, alloc_traits>> m_pool;
};

} // namespace alloc_utility

#endif // POOL_ALLOCATION_HPP
