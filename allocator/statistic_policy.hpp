#ifndef STATISTIC_POLICY_HPP
#define STATISTIC_POLICY_HPP

#include <memory>

#include "alloc_traits.hpp"
#include "alloc_policies.hpp"
#include "macro.hpp"

namespace alloc_utility
{

template <typename alloc_traits>
class simple_statistic
{
public:

    DECLARE_ALLOC_TRAITS(void, alloc_traits)

    size_type allocs_count() const
    {}

    size_type deallocs_count() const
    {}

    size_type allocated_blocks_count() const
    {}

    size_type mem_used() const
    {}

    void register_alloc(const const_void_pointer& ptr, size_type n)
    {}

    void register_dealloc(const const_void_pointer& ptr, size_type n)
    {}

private:
};

template <typename T, typename alloc_traits = allocation_traits<T>,
          typename statistic = simple_statistic<alloc_traits>,
          typename base_policy = none_policy<T>>
class statistic_policy: public base_policy
{
public:

    DECLARE_ALLOC_POLICY_WT(statistic_policy, base_policy, T, alloc_traits)

    typedef statistic statistic_type;

    explicit statistic_policy(statistic* stat = nullptr):
        m_stat(stat)
    {}

    pointer allocate(size_type n, const pointer& ptr, const_void_pointer hint = nullptr)
    {}

    void deallocate(const pointer& ptr, size_type n)
    {}

    statistic* get_statistic() const
    {
        return m_stat;
    }

    void set_statistic(statistic* stat)
    {
        m_stat = stat;
    }

private:
    statistic* m_stat;
};

}

#endif // STATISTIC_POLICY_HPP
