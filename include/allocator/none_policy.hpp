#ifndef NONE_POLICY_HPP
#define NONE_POLICY_HPP

#include "macro.hpp"

namespace alloc_utility
{

template <typename T, typename alloc_traits = allocation_traits<T>>
class none_policy
{
public:

    DECLARE_ALLOC_TRAITS(T, alloc_traits)

    template <typename U>
    using rebind = none_policy<U, typename alloc_traits::template rebind<U>>;
    template <typename policy>
    using rebind_base = none_policy<T, alloc_traits>;

    typedef std::false_type propagate_on_container_copy_assignment;
    typedef std::false_type propagate_on_container_move_assignment;
    typedef std::false_type propagate_on_container_swap;

    none_policy() = default;
    none_policy(const none_policy& other) = default;
    none_policy(none_policy&&) = default;

    template <typename U>
    none_policy(const none_policy::rebind<U>&)
    {}

    none_policy& operator=(const none_policy&) = default;
    none_policy& operator=(none_policy&&) = default;

    void swap(none_policy&) noexcept
    {}

    pointer allocate(size_type n, const pointer& ptr, const_void_pointer hint = nullptr) noexcept
    {
        ALLOC_UNUSED(n);
        ALLOC_UNUSED(hint);
        return ptr;
    }

    void deallocate(const pointer& ptr, size_type n) noexcept
    {
        ALLOC_UNUSED(ptr);
        ALLOC_UNUSED(n);
        return;
    }
};

}

#endif // NONE_POLICY_HPP
