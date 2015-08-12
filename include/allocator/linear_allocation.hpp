#ifndef LINEAR_ALLOCATION_HPP
#define LINEAR_ALLOCATION_HPP

#include <cstdint>
#include <memory>

#include "alloc_traits.hpp"
#include "alloc_policies.hpp"
#include "pointer_cast.hpp"
#include "details/linear_storage.hpp"

namespace alloc_utility
{

template <typename T, typename alloc_traits = allocation_traits<T>,
          typename base_policy = default_allocation_policy<T, alloc_traits>>
class linear_allocation_policy: public base_policy
{
    typedef typename std::pointer_traits<typename alloc_traits::pointer>::template rebind<std::uint8_t> byte_pointer;
    typedef details::linear_storage<byte_pointer, typename alloc_traits::size_type> storage_type;

public:

    DECLARE_ALLOC_TRAITS(T, alloc_traits)
    DECLARE_REBIND_ALLOC(linear_allocation_policy, T, alloc_traits, base_policy)

    typedef std::true_type propagate_on_container_copy_assignment;
    typedef std::true_type propagate_on_container_move_assignment;
    typedef std::true_type propagate_on_container_swap;

    linear_allocation_policy():
        m_storage(std::make_shared<storage_type>())
    {}

    linear_allocation_policy(const linear_allocation_policy& other):
        m_storage(other.m_storage)
    {}

    template <typename U>
    linear_allocation_policy(const rebind<U>& other):
        m_storage(other.m_storage)
    {}

    ~linear_allocation_policy()
    {}

    linear_allocation_policy& operator=(const linear_allocation_policy&) = delete;
    linear_allocation_policy& operator=(linear_allocation_policy&&) = delete;

    bool is_memory_available(size_type size) const noexcept
    {
        return m_storage->is_memory_available(size * sizeof(T));
    }

    size_type storage_size() const noexcept
    {
        return m_storage->storage_size() / sizeof(T);
    }

    pointer get_storage() const noexcept
    {
        return pointer_cast_traits<pointer, byte_pointer>::reinterpet_pcast(m_storage->get_storage());
    }

    void set_storage(const pointer& ptr, size_type size) noexcept
    {
        m_storage->set_storage(pointer_cast_traits<byte_pointer, pointer>::reinterpet_pcast(ptr),
                               size * sizeof(T));
    }

    void allocate_storage(size_type size)
    {
        pointer mem = base_policy::allocate(size, nullptr);
        m_storage->set_storage(pointer_cast_traits<byte_pointer, pointer>::reinterpet_pcast(mem),
                               size * sizeof(T));
    }

    pointer allocate(size_type n, const pointer& ptr, const const_void_pointer& hint = nullptr)
    {
        if (ptr) {
            return ptr;
        }
        if (is_memory_available(n)) {
            return pointer_cast_traits<pointer, byte_pointer>
                    ::reinterpet_pcast(m_storage->allocate(n * sizeof(T)));
        }
        return base_policy::allocate(n, ptr, hint);
    }

    void deallocate(const pointer& ptr, size_type n)
    {
        if (!ptr) {
            return;
        }
        if (m_storage->is_owned(pointer_cast_traits<byte_pointer, pointer>::reinterpet_pcast(ptr))) {
            return;
        }
        base_policy::deallocate(ptr, n);
    }

    bool operator==(const linear_allocation_policy& other) const noexcept
    {
        return m_storage == other.m_storage;
    }

    bool operator!=(const linear_allocation_policy& other) const noexcept
    {
        return !operator==(other);
    }

    template <typename, typename, typename>
    friend class linear_allocation_policy;

private:
    std::shared_ptr<storage_type> m_storage;
};

} // namespace alloc_utility

#endif // LINEAR_ALLOCATION_HPP
