#ifndef POOL_ALLOCATION_HPP
#define POOL_ALLOCATION_HPP

#include <cstdint>
#include <list>
#include <memory>

#include "alloc_traits.hpp"
#include "alloc_policies.hpp"
#include "pointer_cast.hpp"
#include "macro.hpp"
#include "details/memory_pool.hpp"

namespace alloc_utility
{

template <typename T, typename alloc_traits = allocation_traits<T>,
          typename base_policy = default_allocation_policy<T, alloc_traits>>
class pool_allocation_policy: public base_policy
{
    typedef typename std::pointer_traits<typename alloc_traits::pointer>::template rebind<std::uint8_t> byte_pointer;
    typedef details::memory_pool<byte_pointer, typename alloc_traits::size_type> pool_type;
    typedef details::pools_manager<byte_pointer, typename alloc_traits::size_type> pools_manager_type;

public:

    DECLARE_ALLOC_TRAITS(T, alloc_traits)
    DECLARE_REBIND_ALLOC(pool_allocation_policy, T, alloc_traits, base_policy)

    static const size_type DEFAULT_BLOCK_SIZE = pool_type::memory_block_type::chunk_type::CHUNK_MAXSIZE;

    explicit pool_allocation_policy(size_type block_size = DEFAULT_BLOCK_SIZE):
        m_manager(std::make_shared<pools_manager_type>())
      , m_pool(m_manager->get_pool(sizeof(T)))
      , m_block_size(block_size)
    {}

    pool_allocation_policy(const pool_allocation_policy& other):
        base_policy(other)
      , m_manager(other.m_manager)
      , m_pool(m_manager->get_pool(sizeof(T)))
      , m_block_size(other.m_block_size)
    {}

    template <typename U>
    pool_allocation_policy(const rebind<U>& other):
        base_policy(other)
      , m_manager(other.m_manager)
      , m_pool(m_manager->get_pool(sizeof(T)))
      , m_block_size(other.block_size())
    {}

    ~pool_allocation_policy()
    {
        m_manager->release_pool(sizeof(T));
        if (m_manager->get_pool_ref_count(sizeof(T)) == 0) {
            typename pool_type::memory_blocks_range mb_range = m_pool->get_mem_blocks();
            for (auto it = mb_range.begin(); it != mb_range.end(); ++it) {
                pointer ptr = pointer_cast_traits<pointer, byte_pointer>::reinterpet_pointer_cast(it->get_memory_ptr());
                base_policy::deallocate(ptr, it->size());
            }
            m_manager->erase_pool(sizeof(T));
        }
    }

    pool_allocation_policy& operator=(const pool_allocation_policy&) = delete;
    pool_allocation_policy& operator=(pool_allocation_policy&&) = delete;

    size_type capacity() const noexcept
    {
        return m_pool->capacity();
    }

    size_type block_size() const noexcept
    {
        return m_block_size;
    }

    void set_block_size(size_type block_size) noexcept
    {
        assert(block_size > 0);
        m_block_size = block_size;
    }

    void reserve(size_type new_capacity)
    {
        if (new_capacity <= capacity()) {
            return;
        }
        size_type cap_diff = new_capacity - capacity();
        add_mem_block(cap_diff);
    }

    pointer allocate(size_type n, const pointer& ptr, const const_void_pointer& hint = nullptr)
    {
        if (ptr) {
            return ptr;
        }
        if (n > 1 || n == 0) {
            return base_policy::allocate(n, ptr, hint);
        }
        if (!m_pool->is_memory_available()) {
            add_mem_block(m_block_size, hint);
        }
        return pointer_cast_traits<pointer, byte_pointer>::reinterpet_pointer_cast(m_pool->allocate());
    }

    void deallocate(const pointer& ptr, size_type n)
    {
        byte_pointer byte_ptr = pointer_cast_traits<byte_pointer, pointer>::reinterpet_pointer_cast(ptr);
        if (m_pool->is_owned(byte_ptr)) {
            m_pool->deallocate(byte_ptr);
            return;
        }
        for (auto it = m_manager->begin(); it != m_manager->end(); ++it) {
            if (it->obj_size() != sizeof(T) && it->is_owned(byte_ptr)) {
                it->deallocate(byte_ptr);
                return;
            }
        }
        base_policy::deallocate(ptr, n);
    }

    bool operator==(const pool_allocation_policy& other) const noexcept
    {
        return (m_manager == other.m_manager);
    }

    bool operator!=(const pool_allocation_policy& other) const noexcept
    {
        return !operator==(other);
    }

    template <typename, typename, typename>
    friend class pool_allocation_policy;

private:

    void add_mem_block(size_type size, const const_void_pointer& hint = nullptr)
    {
        pointer mem = base_policy::allocate(size, pointer(nullptr), hint);
        m_pool->add_mem_block(pointer_cast_traits<byte_pointer, pointer>::reinterpet_pointer_cast(mem), size);
    }

    std::shared_ptr<pools_manager_type> m_manager;
    pool_type* m_pool;
    size_type m_block_size;
};

} // namespace alloc_utility

#endif // POOL_ALLOCATION_HPP
