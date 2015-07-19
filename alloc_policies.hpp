#ifndef ALLOC_POLICIES_H
#define ALLOC_POLICIES_H

#include <iostream>
#include <new>

#include "alloc_traits.hpp"

namespace alloc_utility
{

template <typename T, typename alloc_traits = allocation_traits<T>>
class none_policy
{
public:

    typedef T value_type;
    typedef typename alloc_traits::pointer pointer;
    typedef typename alloc_traits::const_pointer const_pointer;
    typedef typename alloc_traits::size_type size_type;

    typedef alloc_traits allocation_traits;

    template <typename U>
    using rebind = none_policy<U, typename alloc_traits::template rebind<U>>;

    template <typename policy>
    using rebind_base = none_policy<T, alloc_traits>;

    pointer allocate(size_type n, const pointer& ptr, std::allocator<void>::const_pointer hint)
    {
        return ptr;
    }

    void deallocate(const pointer& ptr, size_type n)
    {
        return;
    }
};

template <typename T, typename alloc_traits = allocation_traits<T>, typename base_policy = none_policy<T>>
class default_allocation_policy: public base_policy
{
public:

    typedef T value_type;
    typedef typename alloc_traits::pointer pointer;
    typedef typename alloc_traits::const_pointer const_pointer;
    typedef typename alloc_traits::size_type size_type;

    typedef alloc_traits allocation_traits;

    template <typename U>
    using rebind = default_allocation_policy<
                                                U,
                                                typename alloc_traits::template rebind<U>,
                                                typename base_policy::template rebind<U>
                                            >;

    template <typename policy>
    using rebind_base = default_allocation_policy<
                                                    T,
                                                    alloc_traits,
                                                    typename policy::template rebind<T>
                                                 >;

    pointer allocate(size_type n, const pointer& ptr, std::allocator<void>::const_pointer hint)
    {
        pointer res = ptr;
        if (!ptr) {
            res = static_cast<pointer>(::operator new(n * sizeof(value_type), std::nothrow));
        }
        res = base_policy::allocate(n, res, hint);
        return res;
    }

    void deallocate(const pointer& ptr, size_type n)
    {
        ::operator delete(static_cast<void*>(ptr));
        base_policy::deallocate(ptr, n);
    }

};

template <typename T, typename alloc_traits = allocation_traits<T>, typename base_policy = none_policy<T>>
class throw_bad_alloc_policy: public base_policy
{
public:

    typedef T value_type;
    typedef typename alloc_traits::pointer pointer;
    typedef typename alloc_traits::const_pointer const_pointer;
    typedef typename alloc_traits::size_type size_type;

    typedef alloc_traits allocation_traits;

    template <typename U>
    using rebind = throw_bad_alloc_policy<
                                            U,
                                            typename alloc_traits::template rebind<U>,
                                            typename base_policy::template rebind<U>
                                         >;


    template <typename policy>
    using rebind_base = throw_bad_alloc_policy<
                                                T,
                                                alloc_traits,
                                                typename policy::template rebind<T>
                                              >;

    pointer allocate(size_type n, const pointer& ptr, std::allocator<void>::const_pointer hint)
    {
        if (!ptr) {
            throw std::bad_alloc();
        }
        base_policy::allocate(n, ptr, hint);
    }

    void deallocate(const pointer& ptr, size_type n)
    {
        base_policy::deallocate(ptr, n);
    }
};

template <typename T, typename alloc_traits = allocation_traits<T>, typename base_policy = none_policy<T>>
class logging_policy: public base_policy
{
public:

    typedef T value_type;
    typedef typename alloc_traits::pointer pointer;
    typedef typename alloc_traits::const_pointer const_pointer;
    typedef typename alloc_traits::size_type size_type;

    typedef alloc_traits allocation_traits;

    template <typename U>
    using rebind = logging_policy<
                                    U,
                                    typename alloc_traits::template rebind<U>,
                                    typename base_policy::template rebind<U>
                                 >;


    template <typename policy>
    using rebind_base = logging_policy<
                                        T,
                                        alloc_traits,
                                        typename policy::template rebind<T>
                                      >;

    logging_policy(std::ostream* log = nullptr):
        m_log(log)
    {}

    pointer allocate(size_type n, const pointer& ptr, std::allocator<void>::const_pointer hint)
    {
        if (ptr && m_log) {
            *m_log << "Allocate at " << ptr << " " << n * sizeof(value_type) << " bytes" << std::endl;
        }
        pointer res = base_policy::allocate(n, ptr, hint);
        return res;
    }

    void deallocate(const pointer& ptr, size_type n)
    {
        if (m_log) {
            *m_log << "Deallocate at " << ptr << " " << n * sizeof(value_type) << " bytes" << std::endl;
        }
        base_policy::deallocate(ptr, n);
    }

    std::ostream* log() const
    {
        return m_log;
    }

    void set_log(std::ostream* log)
    {
        m_log = log;
    }

private:

    std::ostream* m_log;
};


}

#endif // ALLOC_POLICIES_H
