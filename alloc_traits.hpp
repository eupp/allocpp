#ifndef ALLOC_TRAITS_HPP
#define ALLOC_TRAITS_HPP

#include <memory>
#include <type_traits>
#include <limits>
#include <utility>

namespace alloc_utility
{

template <typename T>
class allocation_traits
{
public:

    typedef T value_type;
    typedef typename std::add_pointer<T>::type pointer;
    typedef typename std::add_lvalue_reference<T>::type reference;
    typedef typename std::add_pointer<typename std::add_const<T>::type>::type const_pointer;
    typedef typename std::add_lvalue_reference<typename std::add_const<T>::type>::type const_reference;
    typedef size_t size_type;
    typedef typename std::pointer_traits<pointer>::difference_type difference_type;

    template <typename U>
    using rebind = allocation_traits<U>;

    template <typename U>
    using rebind_pointer = typename std::pointer_traits<pointer>::template rebind<U>;

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
        return std::numeric_limits<size_type>::max();
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
