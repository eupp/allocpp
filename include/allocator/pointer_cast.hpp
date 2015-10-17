#ifndef POINTER_CAST_HPP
#define POINTER_CAST_HPP

#include <memory>

namespace alloc_utility
{

/**
  *
  * for any custom pointer type following definitions should be provided:
  * (1) rebind<U>                        - type alias for pointer<U>
  * (2) static_pointer_cast<T, U>        - static member function which is cast pointer<U> to pointer<T> staticly
  * (3) dynamic_pointer_cast<T, U>       - static member function which is cast pointer<U> to pointer<T> dynamicly
  * (4) reinterpret_pointer_cast<T, U>   - static member function which is return an interpretation of memory pointed by
  *                                        pointer<U> as a an array of objects of type T (and such array is pointed by result).
  */

template <typename pointer>
struct pointer_cast_traits
{
    typedef typename std::pointer_traits<pointer>::element_type element_type;

    template <typename U>
    using rebind = typename std::pointer_traits<pointer>::template rebind<U>;

    template <typename U>
    static pointer static_pcast(const rebind<U>& ptr)
    {
        return pointer::template static_pointer_cast<element_type, U>(ptr);
    }

    template <typename U>
    static pointer dynamic_pcast(const rebind<U>& ptr)
    {
        return pointer::template dynamic_pointer_cast<element_type, U>(ptr);
    }

    template <typename U>
    static pointer reinterpret_pcast(const rebind<U>& ptr)
    {
        return pointer::template reinterpret_pointer_cast<element_type, U>(ptr);
    }
};

// specialization for T*

template <typename T>
struct pointer_cast_traits<T*>
{
    typedef T element_type;

    template <typename U>
    using rebind = U*;

    template <typename U>
    static T* static_pcast(U* ptr)
    {
        return static_cast<T*>(ptr);
    }

    template <typename U>
    static T* dynamic_pcast(U* ptr)
    {
        return dynamic_cast<T*>(ptr);
    }

    template <typename U>
    static T* reinterpret_pcast(U* ptr)
    {
        return reinterpret_cast<T*>(ptr);
    }
};

// specialization for shared_ptr

template <typename T>
struct pointer_cast_traits<std::shared_ptr<T>>
{
    typedef T element_type;

    template <typename U>
    using rebind = std::shared_ptr<U>;

    template <typename U>
    static std::shared_ptr<T> static_pcast(const std::shared_ptr<U>& ptr)
    {
        return std::static_pointer_cast<T, U>(ptr);
    }

    template <typename U>
    static std::shared_ptr<T> dynamic_pcast(const std::shared_ptr<U>& ptr)
    {
        return std::static_pointer_cast<T, U>(ptr);
    }
};



}

#endif // POINTER_CAST_HPP
