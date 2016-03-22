#ifndef ALLOCPP_POINTER_DECORATOR_H
#define ALLOCPP_POINTER_DECORATOR_H

#include <type_traits>
#include <boost/tti/member_type.hpp>

#include <liballocpp/concepts/concepts.hpp>
#include <liballocpp/concepts/pointer_concept_tag.hpp>
#include <liballocpp/utils/pointer.hpp>

namespace allocpp { namespace ptrs {

template <typename Decorated, typename Concept>
class pointer_decorator;

namespace details {
template <typename Concept, typename Decorated>
struct assert_helper
{
    static_assert(std::is_base_of<
                          Concept,
                          typename utils::pointer_traits<Decorated>::concept_tag
                  >::value,
                  "Modeled concept couldn't refine concept of the decorated pointer");
};
}

template <typename Decorated>
class pointer_decorator<Decorated, concepts::object_ptr_tag>
{
    static details::assert_helper<concepts::object_ptr_tag, Decorated> assert_helper;

    BOOST_TTI_MEMBER_TYPE(innermost_type);

    typedef utils::pointer_traits<Decorated> traits;
public:
    typedef typename traits::element_type element_type;
    typedef typename traits::difference_type difference_type;
    typedef typename traits::reference reference;
    typedef typename traits::const_reference const_reference;

    template <typename U>
    using rebind = pointer_decorator<typename traits::template rebind<U>, concepts::object_ptr_tag>;

    typedef Decorated decorated_type;
    typedef typename member_type_innermost_type<Decorated, Decorated>::type innermost_type;
    typedef concepts::object_ptr_tag concept_tag;

    pointer_decorator()
        : m_decorated(nullptr)
    {}

    pointer_decorator(nullptr_t)
        : m_decorated(nullptr)
    {}

    pointer_decorator(const pointer_decorator&) = default;
    pointer_decorator(pointer_decorator&&) = default;

    explicit pointer_decorator(const decorated_type& decorated)
        : m_decorated(decorated)
    {}

    explicit pointer_decorator(decorated_type&& decorated)
        : m_decorated(std::move(decorated))
    {}

    template <typename I = innermost_type, typename = typename std::enable_if<
            std::is_constructible<decorated_type, I>::value
        >::type>
    explicit pointer_decorator(innermost_type p)
        : m_decorated(p)
    {}

    pointer_decorator& operator=(const pointer_decorator&) = default;
    pointer_decorator& operator=(pointer_decorator&&) = default;

    reference operator*()
    {
        return *m_decorated;
    }

    const_reference operator*() const
    {
        return *m_decorated;
    }

    explicit operator bool() const
    {
        return m_decorated != nullptr;
    }

    innermost_type get() const
    {
        return utils::get_ptr(m_decorated);
    }

    decorated_type& decorated_ptr()
    {
        return m_decorated;
    }

    const decorated_type& decorated_ptr() const
    {
        return m_decorated;
    }

    const decorated_type& const_decorated_ptr() const
    {
        return m_decorated;
    }

    friend bool operator==(const pointer_decorator& p1, const pointer_decorator& p2)
    {
        return p1.m_decorated == p2.m_decorated;
    }

    friend bool operator==(const pointer_decorator& p, nullptr_t)
    {
        return p.m_decorated == nullptr;
    }

    friend bool operator==(nullptr_t, const pointer_decorator& p)
    {
        return p.m_decorated == nullptr;
    }

    friend bool operator!=(const pointer_decorator& p1, const pointer_decorator& p2)
    {
        return p1.m_decorated != p2.m_decorated;
    }

    friend bool operator!=(const pointer_decorator& p, nullptr_t)
    {
        return p.m_decorated != nullptr;
    }

    friend bool operator!=(nullptr_t, const pointer_decorator& p)
    {
        return p.m_decorated != nullptr;
    }
protected:
    Decorated m_decorated;
};

template <typename Decorated>
class pointer_decorator<Decorated, concepts::array_ptr_tag>
    : public pointer_decorator<Decorated, concepts::object_ptr_tag>
{
    typedef pointer_decorator<Decorated, concepts::object_ptr_tag> base_type;
public:
    typedef typename base_type::element_type element_type;
    typedef typename base_type::difference_type difference_type;
    typedef typename base_type::reference reference;
    typedef typename base_type::const_reference const_reference;
    using base_type::rebind;

    typedef typename utils::pointer_traits<Decorated>::size_type size_type;

    typedef typename base_type::decorated_type decorated_type;
    typedef typename base_type::innermost_type innermost_type;
    typedef typename base_type::concept_tag concept_tag;

    pointer_decorator()
        : base_type(nullptr)
    {}

    pointer_decorator(nullptr_t)
        : base_type(nullptr)
    {}

    pointer_decorator(const pointer_decorator&) = default;
    pointer_decorator(pointer_decorator&&) = default;

    explicit pointer_decorator(const decorated_type& decorated)
        : base_type(decorated)
    {}

    explicit pointer_decorator(decorated_type&& decorated)
        : base_type(std::move(decorated))
    {}

    template <typename I = innermost_type, typename = typename std::enable_if<
            std::is_constructible<decorated_type, I>::value
    >::type>
    explicit pointer_decorator(innermost_type p)
        : base_type(p)
    {}

    pointer_decorator& operator=(const pointer_decorator&) = default;
    pointer_decorator& operator=(pointer_decorator&&) = default;

    reference operator[](size_type i)
    {
        return this->m_decorated[i];
    }

    const_reference operator[](size_type i) const
    {
        return this->m_decorated[i];
    }
};

}}

#endif //ALLOCPP_POINTER_DECORATOR_H
