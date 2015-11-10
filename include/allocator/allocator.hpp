#ifndef ALLOCATOR_H
#define ALLOCATOR_H

#include <memory>
#include <type_traits>

#include "allocation_traits.hpp"
#include "macro_traits.hpp"
#include "details/policies_list.hpp"

namespace alloc_utility
{

template <typename T, typename alloc_traits, typename... alloc_policies>
class allocator: public details::policies_list<alloc_traits, alloc_policies...>
{
    typedef details::policies_list<alloc_traits, alloc_policies...> base;

public:

    static_assert(sizeof...(alloc_policies) > 0,
                  "Allocator needs at least one allocation policy");

    DECLARE_ALLOC_TRAITS(alloc_traits)

    template <typename U>
    using rebind_t = allocator<
                                U,
                                typename alloc_traits::template rebind<U>,
                                typename alloc_policies::template rebind<U>...
                              >;

    template <typename U>
    struct rebind
    {
        typedef rebind_t<U> other;
    };

    typedef typename base::propagate_on_container_copy_assignment propagate_on_container_copy_assignment;
    typedef typename base::propagate_on_container_move_assignment propagate_on_container_move_assignment;
    typedef typename base::propagate_on_container_swap propagate_on_container_swap;

    allocator() = default;
    allocator(const allocator& other) = default;
    allocator(allocator&& other) = default;

    template <typename U>
    allocator(const rebind_t<U>& other):
        base(other)
    {}

    allocator select_on_container_copy_construction() const
    {
        return allocator(*this);
    }

    allocator& operator=(const allocator& other) = default;
    allocator& operator=(allocator&&) = default;

    void swap(allocator& other) noexcept
    {
        base::swap(other);
    }

    pointer address(reference ref) const noexcept
    {
        return alloc_traits::address(ref);
    }

    const_pointer address(const_reference ref) const noexcept
    {
        return alloc_traits::address(ref);
    }

    pointer allocate(size_type n, const_void_pointer hint = nullptr)
    {
        return base::allocate(n, nullptr, hint);
    }

    void deallocate(const pointer& ptr, size_type n)
    {
        base::deallocate(ptr, n);
    }

    size_type max_size() const noexcept
    {
        return alloc_traits::max_size();
    }

    template <typename U, typename... Args>
    void construct(rebind_pointer<U> ptr, Args&&... args)
    {
        alloc_traits::construct(ptr, std::forward<Args>(args)...);
    }

    template <typename U>
    void destroy(rebind_pointer<U> ptr)
    {
        alloc_traits::destroy(ptr);
    }

    bool operator==(const allocator& other) const noexcept
    {
        return base::operator==(other);
    }

    bool operator!=(const allocator& other) const noexcept
    {
        return base::operator!=(other);
    }
};

template <typename T, typename alloc_traits, typename... alloc_policies>
void swap(allocator<T, alloc_traits, alloc_policies...>& alloc1,
          allocator<T, alloc_traits, alloc_policies...>& alloc2) noexcept
{
    alloc1.swap(alloc2);
}

template <typename T, typename alloc_traits, typename... alloc_policies>
bool operator==(const allocator<T, alloc_traits, alloc_policies...>& alloc1,
                const allocator<T, alloc_traits, alloc_policies...>& alloc2) noexcept
{
    return alloc1.operator==(alloc2);
}

template <typename T, typename alloc_traits, typename... alloc_policies>
bool operator!=(const allocator<T, alloc_traits, alloc_policies...>& alloc1,
                const allocator<T, alloc_traits, alloc_policies...>& alloc2) noexcept
{
    return alloc1.operator!=(alloc2);
}

} // namespace alloc_utility

#endif // ALLOCATOR_H
