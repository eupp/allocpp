#ifndef ALLOCATOR_H
#define ALLOCATOR_H

#include <memory>
#include <type_traits>

#include "alloc_traits.hpp"

namespace alloc_utility
{

namespace details
{
    // conversation from constant to type
    // will be useful further

    template <bool b>
    struct bool_to_type
    {
        typedef std::true_type type;
    };

    template <>
    struct bool_to_type<false>
    {
        typedef std::false_type type;
    };

    // helper class for iterate through policies
    // it keeps information about types of policies

    template <typename alloc, typename... alloc_policies>
    class policies_list
    {
    public:

        typedef typename alloc::allocation_traits alloc_traits;
        typedef typename alloc_traits::pointer pointer;
        typedef typename alloc_traits::size_type size_type;

        static pointer allocate(alloc* allocator,
                                size_type n, const pointer& ptr, std::allocator<void>::const_pointer hint = 0);

        static void deallocate(alloc* allocator,
                               const pointer& ptr, size_type n);
    };

    template <typename alloc, typename alloc_policy, typename... alloc_policies>
    class policies_list<alloc, alloc_policy, alloc_policies...>
    {
    public:

        typedef typename alloc::allocation_traits alloc_traits;
        typedef typename alloc_traits::pointer pointer;
        typedef typename alloc_traits::size_type size_type;

        static pointer allocate(alloc* allocator,
                                size_type n, const pointer& ptr, std::allocator<void>::const_pointer hint = 0)
        {
            allocator->alloc_policy::allocate(n, ptr, allocate_callback);
        }

        // callback invokes allocate method from next policy
        static pointer allocate_callback(alloc_policy* policy,
                                         size_type n, const pointer& ptr, std::allocator<void>::const_pointer hint = 0)
        {
            alloc* allocator = static_cast<alloc*>(policy);
            return policies_list<alloc, alloc_policies>::allocate(allocator, n, ptr, hint);
        }

        static void deallocate(alloc* allocator,
                               const pointer& ptr, size_type n)
        {
            allocator->alloc_policy::deallocate(n, ptr, deallocate_callback);
        }

        // callback invokes deallocate method from next policy
        static void deallocate_callback(alloc_policy* policy,
                                        const pointer& ptr, size_type n)
        {
            alloc* allocator = static_cast<alloc*>(policy);
            return policies_list<alloc, alloc_policies>::deallocate(allocator, ptr, n);
        }
    };

    template <typename alloc>
    class policies_list<alloc>
    {
    public:

        typedef typename alloc::allocation_traits alloc_traits;
        typedef typename alloc_traits::pointer pointer;
        typedef typename alloc_traits::size_type size_type;

        static pointer allocate(alloc* allocator,
                                size_type n, const pointer& ptr, std::allocator<void>::const_pointer hint = 0)
        {
            return ptr;
        }

        static void deallocate(alloc* allocator,
                               const pointer& ptr, size_type n)
        {
            return;
        }
    };

}

template <typename T, typename alloc_traits, typename... alloc_policies>
class allocator: public alloc_policies...
{
public:

    static_assert(sizeof...(alloc_policies) > 0,
                  "Allocator needs at least one allocation policy");

    typedef T value_type;
    typedef typename alloc_traits::pointer pointer;
    typedef typename alloc_traits::reference reference;
    typedef typename alloc_traits::const_pointer const_pointer;
    typedef typename alloc_traits::const_reference const_reference;
    typedef typename alloc_traits::size_type size_type;
    typedef typename alloc_traits::difference_type difference_type;

    typedef alloc_traits allocation_traits;

    template <typename U>
    struct rebind
    {
        typedef allocator<
                          U,
                          typename alloc_traits::template rebind<U>,
                          typename alloc_policies::template rebind<U>...
                         > other;
    };

    template <typename U>
    using rebind_pointer = typename alloc_traits::template rebind_pointer<U>;

    pointer address(reference ref) const noexcept
    {
        return alloc_traits::address(ref);
    }

    const_pointer address(const_reference ref) const noexcept
    {
        return alloc_traits::address(ref);
    }

    pointer allocate(size_type n, std::allocator<void>::const_pointer hint = 0)
    {
        return allocate_helper<alloc_policies...>::allocate(*this, n, nullptr, hint);
    }

    void deallocate(const pointer& ptr, size_type n)
    {
        return allocate_helper<alloc_policies...>::deallocate(*this, ptr, n);
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
};

}

#endif // ALLOCATOR_H
