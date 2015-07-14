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

// trait for check existance of allocate function in policy

template <typename C>
class is_allocate_defined
{

    template <
              typename X,
              typename C::allocation_traits::pointer (X::*)
                        (
                         typename C::allocation_traits::size_type n,
                         const typename C::allocation_traits::pointer&,
                         std::allocator<void>::const_pointer
                        ) = &X::allocate
             >
    struct check_helper {};

    typedef char true_type;
    struct false_type { true_type m[2]; };

    template <typename X>
    static true_type check(check_helper<X>* p);

    template <typename X>
    static false_type check(...);

public:

    static bool const value = sizeof((check<C>)(0)) == sizeof(true_type);
};

// trait for check existance of deallocate function in policy

template <typename C, typename alloc_traits = allocation_traits<C>>
class is_deallocate_defined
{
    typedef typename C::allocation_traits::pointer pointer;
    typedef typename C::allocation_traits::size_type size_type;

    template <
              typename X,
              void (X::*)
                         (
                          const typename C::allocation_traits::pointer& ptr,
                          typename C::allocation_traits::size_type n
                         ) = &X::deallocate
             >
    struct check_helper {};

    typedef char true_type;
    struct false_type { true_type m[2]; };

    template <typename X>
    static true_type check(check_helper<X>* p);

    template <typename X>
    static false_type check(...);

public:

    static bool const value = sizeof((check<C>)(0)) == sizeof(true_type);
};

}

#endif // ALLOC_TRAITS_HPP
