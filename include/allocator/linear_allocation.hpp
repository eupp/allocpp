#ifndef LINEAR_ALLOCATION_HPP
#define LINEAR_ALLOCATION_HPP

#include "alloc_traits.hpp"
#include "alloc_policies.hpp"

namespace alloc_utility
{

template <typename T, typename alloc_traits = allocation_traits<T>,
          typename base_policy = default_allocation_policy<T, alloc_traits>>
class linear_allocation_policy
{
public:

    DECLARE_ALLOC_TRAITS(T, alloc_traits)
    DECLARE_REBIND_ALLOC(linear_allocation_policy, T, alloc_traits, base_policy)

    typedef std::true_type propagate_on_container_copy_assignment;
    typedef std::true_type propagate_on_container_move_assignment;
    typedef std::true_type propagate_on_container_swap;

    linear_allocation_policy()
    {}

    linear_allocation_policy(const linear_allocation_policy& other)
    {}

    template <typename U>
    linear_allocation_policy(const rebind<U>& other)
    {}

    ~linear_allocation_policy()
    {}

    linear_allocation_policy& operator=(const linear_allocation_policy&) = delete;
    linear_allocation_policy& operator=(linear_allocation_policy&&) = delete;

    size_type storage_size() const noexcept
    {}

    pointer get_storage() const
    {}

    void set_storage(const pointer& ptr, size_type size)
    {}

    void allocate_storage(size_type size)
    {}

    pointer allocate(size_type n, const pointer& ptr, const const_void_pointer& hint = nullptr)
    {}

    void deallocate(const pointer& ptr, size_type n)
    {}

    bool operator==(const linear_allocation_policy& other) const noexcept
    {}

    bool operator!=(const linear_allocation_policy& other) const noexcept
    {
        return !operator==(other);
    }

    template <typename, typename, typename>
    friend class linear_allocation_policy;

private:

};

} // namespace alloc_utility

#endif // LINEAR_ALLOCATION_HPP
