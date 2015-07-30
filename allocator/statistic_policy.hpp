#ifndef STATISTIC_POLICY_HPP
#define STATISTIC_POLICY_HPP

#include <memory>

#include "alloc_traits.hpp"
#include "alloc_policies.hpp"
#include "pointer_cast.hpp"
#include "macro.hpp"

namespace alloc_utility
{

template <typename alloc_traits>
class simple_statistic
{
public:

    DECLARE_ALLOC_TRAITS(void, alloc_traits)

    simple_statistic() noexcept:
        m_allocs_count(0)
      , m_deallocs_count(0)
      , m_mem_used(0)
    {}

    size_type allocs_count() const noexcept
    {
        return m_allocs_count;
    }

    size_type deallocs_count() const noexcept
    {
        return m_deallocs_count;
    }

    size_type allocated_blocks_count() const noexcept
    {
        return m_allocs_count - m_deallocs_count;
    }

    size_type mem_used() const noexcept
    {
        return m_mem_used;
    }

    void register_alloc(const const_void_pointer& ptr, size_type n) noexcept
    {
        ALLOC_UNUSED(ptr);
        ++m_allocs_count;
        m_mem_used += n;
    }

    void register_dealloc(const const_void_pointer& ptr, size_type n) noexcept
    {
        ALLOC_UNUSED(ptr);
        ++m_deallocs_count;
        m_mem_used -= n;
    }

private:
    size_type m_allocs_count;
    size_type m_deallocs_count;
    size_type m_mem_used;
};

template <typename T, typename alloc_traits = allocation_traits<T>,
          typename statistic = simple_statistic<alloc_traits>,
          typename base_policy = none_policy<T>>
class statistic_policy: public base_policy
{
public:

    DECLARE_ALLOC_TRAITS(T, alloc_traits)
    DECLARE_REBIND_ALLOC(statistic_policy, T, alloc_traits, statistic, base_policy)

    typedef statistic statistic_type;

    explicit statistic_policy(statistic* stat = nullptr) noexcept:
        m_stat(stat)
    {}

    pointer allocate(size_type n, const pointer& ptr, const_void_pointer hint = nullptr)
    {
        if (m_stat) {
            const_void_pointer void_ptr = pointer_cast_traits<const_void_pointer, pointer>::static_pointer_cast(ptr);
            m_stat->register_alloc(void_ptr, n * sizeof(T));
        }
        return base_policy::allocate(n, ptr, hint);
    }

    void deallocate(const pointer& ptr, size_type n)
    {
        if (m_stat) {
            const_void_pointer void_ptr = pointer_cast_traits<const_void_pointer, pointer>::static_pointer_cast(ptr);
            m_stat->register_dealloc(void_ptr, n * sizeof(T));
        }
        base_policy::deallocate(ptr, n);
    }

    statistic* get_statistic() const noexcept
    {
        return m_stat;
    }

    void set_statistic(statistic* stat) noexcept
    {
        m_stat = stat;
    }

private:
    statistic* m_stat;
};

}

#endif // STATISTIC_POLICY_HPP
