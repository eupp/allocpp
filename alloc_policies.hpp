#ifndef ALLOC_POLICIES_H
#define ALLOC_POLICIES_H

#include <iostream>

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

    template <typename U>
    using rebind_pointer = typename std::pointer_traits<pointer>::template rebind<U>;

    pointer allocate(size_type n, const pointer& ptr, std::allocator<void>::const_pointer hint = 0)
    {
        if (!ptr) {
            return reinterpret_cast<pointer>(::operator new(n * sizeof(value_type), std::nothrow));
        }
        return ptr;
    }

    void deallocate(const pointer& p, size_type n)
    {
        ::operator delete(reinterpret_cast<void*>(p));
    }

};

template <typename T, typename alloc_traits = allocation_traits<T>>
class throw_bad_alloc_policy
{
public:


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

    template <typename U>
    using rebind_pointer = typename std::pointer_traits<pointer>::template rebind<U>;

    logging_policy(std::ostream* log = nullptr):
        m_log(log)
    {}

    pointer allocate(size_type n, const pointer& ptr, std::allocator<void>::const_pointer hint = 0)
    {
        if (ptr && m_log) {
            *m_log << "Allocate at " << ptr << " " << n * sizeof(value_type) << " bytes" << std::endl;
        }
        return ptr;
    }

    void deallocate(const pointer& ptr, size_type n)
    {
        if (m_log) {
            *m_log << "Deallocate at " << ptr << " " << n * sizeof(value_type) << " bytes" << std::endl;
        }
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
