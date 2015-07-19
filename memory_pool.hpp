#ifndef MEMORY_POOL_HPP
#define MEMORY_POOL_HPP

#include <cstdint>
#include <vector>

#include "alloc_traits.hpp"
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

    template <typename U>
    using rebind = chunk<U, typename alloc_traits::template rebind<U>>;

    template <typename U>
    using rebind_pointer = typename alloc_traits::template rebind_pointer<U>;

    typedef rebind_pointer<uint8_t> raw_pointer;

    static const int CHUNK_SIZE = std::UINT8_MAX;

    chunk(const pointer& chunk_ptr):
        m_chunk(chunk_ptr)
      , m_head(0)
      , m_available(CHUNK_SIZE)
    {
        pointer ptr = m_chunk;
        for (int i = 1; i <= CHUNK_SIZE; ++i, ++ptr) {
            raw_pointer bytes = pointer_cast_traits<raw_pointer, pointer>::reinterpet_pointer_cast(ptr);
            bytes[0] = i;
        }
        // last block is initialized with CHUNK_SIZE
        // it is really doesn't matter because when we reach last block m_available == 0
    }

    pointer get_pointer() const
    {
        return m_chunk;
    }

    bool is_memory_available() const
    {
        return m_available > 0;
    }

    bool is_empty() const
    {
        return m_available == CHUNK_SIZE;
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

template <typename T, typename alloc_traits = allocation_traits<T>>
class pool_allocation_policy
{
public:

    typedef T value_type;
    typedef typename alloc_traits::pointer pointer;
    typedef typename alloc_traits::const_pointer const_pointer;
    typedef typename alloc_traits::size_type size_type;

    typedef alloc_traits allocation_traits;

    template <typename U>
    using rebind = pool_allocation_policy<U>;

    template <typename allocator>
    pointer allocate(allocator& alloc,
                     size_type n, const pointer& ptr, std::allocator<void>::const_pointer hint = nullptr)
    {
        if (ptr) {
            return ptr;
        }
        if (n > 1) {
            return alloc.allocate(n, ptr, hint);
        }
        for (auto& chunk: m_pool) {
            if (chunk.is_memory_available()) {
                return chunk.allocate();
            }
        }
        m_pool.emplace_back(alloc.allocate(details::chunk::CHUNK_SIZE));
        return m_pool.back().allocate();
    }

    template <typename allocator>
    void deallocate(allocator& alloc,
                    const pointer& ptr, size_type n)
    {
        if (!ptr) {
            return;
        }
        if (n > 1) {
            alloc.deallocate(ptr, n);
        }
        for (auto& chunk: m_pool) {
            if (chunk.is_owned(ptr)) {
                chunk.deallocate(ptr);
                if (chunk.is_empty()) {

                }
            }
        }
    }

private:
    std::vector<details::chunk> m_pool;
};

} // namespace alloc_utility

#endif // MEMORY_POOL_HPP
