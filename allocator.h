#ifndef ALLOCATOR_H
#define ALLOCATOR_H

#include <memory>
#include <type_traits>
#include <tuple>
#include <utility>

namespace alloc_utility
{

namespace details
{

template <typename... policies>
struct policies_list;

template <typename policy>
struct policies_list<policy>
{
    typedef policy type;
    type m_policy;

    policies_list(policy&& p):
        m_policy(p)
    {}
};

template <typename policy, typename... policies>
struct policies_list<policy, policies...>
{
    typedef policy type;
    type m_policy;
    policies_list<policies...> m_other;

    policies_list(policy&& p, policies&&... ps):
        m_policy(p)
      , m_other(ps)
    {}
};

}

template <typename T, typename allocation_traits, typename... policies>
class allocator
{
public:

    typedef T value_type;
    typedef typename std::add_pointer<T>::type pointer;
    typedef typename std::add_lvalue_reference<T>::type reference;
    typedef typename std::add_pointer<std::add_const<T>::type>::type const_pointer;
    typedef typename std::add_lvalue_reference<std::add_const<T>::type>::type const_reference;
    typedef typename allocation_traits::size_type size_type;
    typedef typename std::pointer_traits<pointer>::difference_type difference_type;

    template <typename U>
    struct rebind
    {
        typedef allocator<U> other;
    };

    pointer address(reference ref) const noexcept
    {
        return allocation_traits::addressof(ref);
    }

    const_pointer address(const_reference ref) const noexcept
    {
        return allocation_traits::addressof(ref);
    }

    pointer allocate(size_type n, typename std::allocator<void>::const_pointer hint = 0);

    void deallocate(pointer p, size_type n);

    size_type max_size() const noexcept
    {
        return allocation_traits::max_size();
    }

    template <typename U, typename... Args>
    void construct(std::add_pointer<U>::type p, Args&&... args)
    {
        allocation_traits::construct(p, std::forward<Args>(args)...);
    }

    template <typename U>
    void destroy(std::add_pointer<U>::type p)
    {
        allocation_traits::destroy(p);
    }

private:
    std::tuple<policies...> m_policies;

};

}

#endif // ALLOCATOR_H
