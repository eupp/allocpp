#ifndef ALLOCATOR_H
#define ALLOCATOR_H

#include <memory>
#include <type_traits>
#include <limits>
#include <tuple>
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
    using rebind_pointer = typename std::pointer_traits<pointer>::template rebind<U>;

    template <typename U>
    using rebind = allocation_traits<U>;

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

template <typename T, typename alloc_traits /*, typename... policies*/>
class allocator
{
public:

    typedef T value_type;
    typedef typename alloc_traits::pointer pointer;
    typedef typename alloc_traits::reference reference;
    typedef typename alloc_traits::const_pointer const_pointer;
    typedef typename alloc_traits::const_reference const_reference;
    typedef typename alloc_traits::size_type size_type;
    typedef typename alloc_traits::difference_type difference_type;

    template <typename U>
    using rebind_pointer = typename alloc_traits::template rebind_pointer<U>;

    template <typename U>
    struct rebind
    {
        typedef allocator<
                          U,
                          typename alloc_traits::template rebind<U> /*,
                          typename policies::rebind<U>::other...*/
                         > other;
    };

    pointer address(reference ref) const noexcept
    {
        return alloc_traits::address(ref);
    }

    const_pointer address(const_reference ref) const noexcept
    {
        return alloc_traits::address(ref);
    }

    pointer allocate(size_type n, typename std::allocator<void>::const_pointer hint = 0)
    {
        return reinterpret_cast<pointer>(::operator new(n * sizeof(value_type)));
    }

    void deallocate(pointer p, size_type n)
    {
        ::operator delete(reinterpret_cast<void*>(p));
    }

    size_type max_size() const noexcept
    {
        return alloc_traits::max_size();
    }

    template <typename U, typename... Args>
    void construct(rebind_pointer<U> p, Args&&... args)
    {
        alloc_traits::construct(p, std::forward<Args>(args)...);
    }

    template <typename U>
    void destroy(rebind_pointer<U> p)
    {
        alloc_traits::destroy(p);
    }

private:

//    template <typename... policies>
//    pointer allocate(size_type n, const pointer& ptr, const std::allocator<void>::const_pointer& hint);

//    template <>
//    pointer allocate<>(size_type n, const pointer& ptr, const std::allocator<void>::const_pointer& hint)
//    {
//        return ptr;
//    }

//    template <typename policy, typename... policies>
//    pointer allocate<policy, policies...>(size_type n, const pointer& ptr, const std::allocator::const_pointer& hint)
//    {
//        pointer new_ptr = policy::allocate(n, ptr, hint);
//        return allocate<policies...>(n, new_ptr, hint);
//    }

//    std::tuple<policies...> m_policies;

};

}

#endif // ALLOCATOR_H
