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

    template <typename alloc_traits, typename... alloc_policies>
    class policies_list
    {};

    template <typename alloc_traits, typename alloc_policy, typename... alloc_policies>
    class policies_list<alloc_traits, alloc_policy, alloc_policies...>
            : public alloc_policy::template rebind_base<policies_list<alloc_traits, alloc_policies...>>
    {
    public:

        typedef typename alloc_traits::pointer pointer;
        typedef typename alloc_traits::size_type size_type;

        typedef typename alloc_policy::template rebind_base<policies_list<alloc_traits, alloc_policies...>> base;

        template <typename U>
        using rebind = policies_list<
                                        typename alloc_traits::template rebind<U>,
                                        typename alloc_policy::template rebind<U>,
                                        typename alloc_policies::template rebind<U>...
                                    >;


        pointer allocate(size_type n, const pointer& ptr, std::allocator<void>::const_pointer hint = 0)
        {
            return this->base::allocate(n, ptr, hint);
        }

        void deallocate(const pointer& ptr, size_type n)
        {
            this->base::deallocate(ptr, n);
        }
    };

    template <typename alloc_traits>
    class policies_list<alloc_traits>
    {
    public:

        typedef typename alloc_traits::pointer pointer;
        typedef typename alloc_traits::size_type size_type;

        template <typename U>
        using rebind = policies_list<typename alloc_traits::template rebind<U>>;

        pointer allocate(size_type n, const pointer& ptr, std::allocator<void>::const_pointer hint = 0)
        {
            return ptr;
        }

        void deallocate(const pointer& ptr, size_type n)
        {
            return;
        }
    };

} // namespace details

template <typename T, typename alloc_traits, typename... alloc_policies>
class allocator: public details::policies_list<alloc_traits, alloc_policies...>
{
    typedef details::policies_list<alloc_traits, alloc_policies...> base;

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
};

} // namespace alloc_utility

#endif // ALLOCATOR_H
