#ifndef ALLOC_POLICIES_H
#define ALLOC_POLICIES_H

#include <iostream>
#include <new>

#include "alloc_traits.hpp"
#include "none_policy.hpp"
#include "macro.hpp"

namespace alloc_utility
{

template <typename T, typename alloc_traits = allocation_traits<T>,
          typename base_policy = none_policy<T, alloc_traits>>
class default_allocation_policy: public base_policy
{
public:

    DECLARE_ALLOC_TRAITS(T, alloc_traits)
    DECLARE_REBIND_ALLOC(default_allocation_policy, T, alloc_traits, base_policy)

    default_allocation_policy() = default;

    template <typename U>
    default_allocation_policy(const rebind<U>& other):
        base_policy(other)
    {
        ALLOC_UNUSED(other);
    }

    pointer allocate(size_type n, const pointer& ptr, const_void_pointer hint = nullptr)
    {
        pointer res = ptr;
        if (!ptr) {
            res = static_cast<pointer>(::operator new(n * sizeof(value_type), std::nothrow));
        }
        return base_policy::allocate(n, res, hint);
    }

    void deallocate(const pointer& ptr, size_type n)
    {
        if (ptr) {
            ::operator delete(static_cast<void*>(ptr));
            base_policy::deallocate(ptr, n);
        }
    }
};

template <typename T, typename alloc_traits = allocation_traits<T>,
          typename base_policy = none_policy<T, alloc_traits>>
class throw_bad_alloc_policy: public base_policy
{
public:

    DECLARE_ALLOC_TRAITS(T, alloc_traits)
    DECLARE_REBIND_ALLOC(throw_bad_alloc_policy, T, alloc_traits, base_policy)

    throw_bad_alloc_policy() = default;

    template <typename U>
    throw_bad_alloc_policy(const rebind<U>& other):
        base_policy(other)
    {
        ALLOC_UNUSED(other);
    }

    pointer allocate(size_type n, const pointer& ptr, const_void_pointer hint = nullptr)
    {
        if (!ptr) {
            throw std::bad_alloc();
        }
        return base_policy::allocate(n, ptr, hint);
    }
};

template <typename T, typename alloc_traits = allocation_traits<T>,
          typename base_policy = none_policy<T, alloc_traits>>
class logging_policy: public base_policy
{
public:

    DECLARE_ALLOC_TRAITS(T, alloc_traits)
    DECLARE_REBIND_ALLOC(logging_policy, T, alloc_traits, base_policy)

    logging_policy(std::ostream* log = nullptr):
        m_log(log)
    {}

    template <typename U>
    logging_policy(const rebind<U>& other):
        base_policy(other)
      , m_log(other.m_log)
    {}

    pointer allocate(size_type n, const pointer& ptr, const_void_pointer hint = nullptr)
    {
        if (ptr && m_log) {
            *m_log << "Allocate at " << ptr << " " << n * sizeof(value_type) << " bytes" << std::endl;
        }
        return base_policy::allocate(n, ptr, hint);
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

    template <typename, typename, typename>
    friend class logging_policy;

private:
    std::ostream* m_log;
};


}

#endif // ALLOC_POLICIES_H
