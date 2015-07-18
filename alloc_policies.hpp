#ifndef ALLOC_POLICIES_H
#define ALLOC_POLICIES_H

#include <iostream>
#include <new>

#include "alloc_traits.hpp"

namespace alloc_utility
{

template <typename T, typename alloc_traits = allocation_traits<T>>
class default_allocation_policy
{
public:

    typedef T value_type;
    typedef typename alloc_traits::pointer pointer;
    typedef typename alloc_traits::const_pointer const_pointer;
    typedef typename alloc_traits::size_type size_type;

    typedef alloc_traits allocation_traits;

    template <typename U>
    using rebind = default_allocation_policy<U>;

    template <typename next_policy>
    pointer allocate(size_type n, const pointer& ptr, std::allocator<void>::const_pointer hint,
                     next_policy policy)
    {
        pointer res = ptr;
        if (!ptr) {
            res = static_cast<pointer>(::operator new(n * sizeof(value_type), std::nothrow));
        }
        res = policy(this, n, res, hint);
        return res;
    }

    template <typename next_policy>
    void deallocate(const pointer& ptr, size_type n,
                    next_policy policy)
    {
        ::operator delete(static_cast<void*>(ptr));
        policy(this, ptr, n);
    }

};

template <typename T, typename alloc_traits = allocation_traits<T>>
class throw_bad_alloc_policy
{
public:

    typedef T value_type;
    typedef typename alloc_traits::pointer pointer;
    typedef typename alloc_traits::const_pointer const_pointer;
    typedef typename alloc_traits::size_type size_type;

    typedef alloc_traits allocation_traits;

    template <typename U>
    using rebind = throw_bad_alloc_policy<U>;

    template <typename next_policy>
    pointer allocate(size_type n, const pointer& ptr, std::allocator<void>::const_pointer hint,
                     next_policy policy)
    {
        if (!ptr) {
            throw std::bad_alloc();
        }
    }

    template <typename next_policy>
    void deallocate(const pointer& ptr, size_type n,
                    next_policy policy)
    {
        policy(this, ptr, n);
    }
};

template <typename T, typename alloc_traits = allocation_traits<T>>
class logging_policy
{
public:

    typedef T value_type;
    typedef typename alloc_traits::pointer pointer;
    typedef typename alloc_traits::const_pointer const_pointer;
    typedef typename alloc_traits::size_type size_type;

    typedef alloc_traits allocation_traits;

    template <typename U>
    using rebind = logging_policy<U>;

    logging_policy(std::ostream* log = nullptr):
        m_log(log)
    {}

    template <typename next_policy>
    pointer allocate(size_type n, const pointer& ptr, std::allocator<void>::const_pointer hint,
                     next_policy policy)
    {
        if (ptr && m_log) {
            *m_log << "Allocate at " << ptr << " " << n * sizeof(value_type) << " bytes" << std::endl;
        }
        pointer res = policy(this, n, ptr, hint);
        return res;
    }

    template <typename next_policy>
    void deallocate(const pointer& ptr, size_type n,
                    next_policy policy)
    {
        if (m_log) {
            *m_log << "Deallocate at " << ptr << " " << n * sizeof(value_type) << " bytes" << std::endl;
        }
        policy(this, ptr, n);
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
