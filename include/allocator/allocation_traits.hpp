#ifndef ALLOC_TRAITS_HPP
#define ALLOC_TRAITS_HPP

#include <memory>
#include <type_traits>
#include <limits>
#include <utility>

namespace alloc_utility
{

template <typename pointer_type>
class allocation_void_traits
{
public:

    typedef typename std::pointer_traits<pointer_type>::template rebind<void> void_pointer;
    typedef typename std::pointer_traits<pointer_type>::template rebind<const void> const_void_pointer;
    typedef size_t size_type;
    typedef typename std::pointer_traits<pointer_type>::difference_type difference_type;

    static size_type max_size() noexcept
    {
        return std::numeric_limits<size_type>::max();
    }
};

template <typename pointer_type>
class allocation_traits
{
public:

    typedef typename std::pointer_traits<pointer_type>::element_type value_type;

    typedef pointer_type pointer;
    typedef typename std::pointer_traits<pointer>::template rebind<const value_type> const_pointer;

    typedef allocation_void_traits<pointer_type> void_traits;

    typedef typename void_traits::void_pointer void_pointer;
    typedef typename void_traits::const_void_pointer const_void_pointer;
    typedef typename void_traits::size_type size_type;
    typedef typename void_traits::difference_type difference_type;

    typedef typename std::add_lvalue_reference<value_type>::type reference;
    typedef typename std::add_lvalue_reference<typename std::add_const<value_type>::type>::type const_reference;

    template <typename U>
    using rebind_pointer = typename std::pointer_traits<pointer>::template rebind<U>;

    template <typename U>
    using rebind_const_pointer = typename std::pointer_traits<const_pointer>::template rebind<U>;

    template <typename U>
    using rebind = allocation_traits<rebind_pointer<U>>;


    static pointer address(reference ref) noexcept
    {
        return std::addressof(ref);
    }

    static const_pointer address(const_reference ref) noexcept
    {
        return std::addressof(ref);
    }

    static size_type max_size() noexcept
    {
        return void_traits::max_size() / sizeof(value_type);
    }

    template <typename U, typename... Args>
    static void construct(rebind_pointer<U> p, Args&&... args)
    {
        ::new (static_cast<void*>(p)) U (std::forward<Args>(args)...);
    }

    template <typename U>
    static void destroy(rebind_pointer<U> p)
    {
        p->~U();
    }
};

}

#endif // ALLOC_TRAITS_HPP
