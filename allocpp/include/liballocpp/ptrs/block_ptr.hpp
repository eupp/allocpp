#ifndef ALLOCPP_BLOCK_PTR_H
#define ALLOCPP_BLOCK_PTR_H

#include <liballocpp/concepts/pointer_concept_tag.hpp>
#include <liballocpp/utils/pointer.hpp>
#include <liballocpp/ptrs/pointer_decorator.hpp>

namespace allocpp { namespace ptrs {

template <typename Ptr>
class block_ptr : public pointer_decorator<Ptr, concepts::array_ptr_tag>
{
    typedef pointer_decorator<Ptr, concepts::array_ptr_tag> base_type;
public:
    typedef typename base_type::element_type element_type;
    typedef typename base_type::difference_type difference_type;
    typedef typename base_type::reference reference;
    typedef typename base_type::const_reference const_reference;
    using base_type::rebind;

    typedef typename utils::pointer_traits<Ptr>::size_type size_type;

    typedef typename base_type::innermost_type innermost_type;

    block_ptr()
        : base_type(nullptr)
        , m_size(0)
    {}

    block_ptr(nullptr_t)
        : block_ptr()
    {}

    block_ptr(const block_ptr&) = default;
    block_ptr(block_ptr&&) = default;

    block_ptr(const Ptr& p, size_type size)
        : base_type(p)
        , m_size(size)
    {}

    explicit block_ptr(Ptr&& p, size_type size)
        : base_type(std::move(p))
        , m_size(size)
    {}

    template <typename I = innermost_type, typename = typename std::enable_if<
            std::is_constructible<Ptr, I>::value
        >::type>
    explicit block_ptr(innermost_type p, size_type size)
        : base_type(p)
        , m_size(size)
    {}

    block_ptr& operator=(const block_ptr&) = default;
    block_ptr& operator=(block_ptr&&) = default;

    Ptr ptr() const
    {
        return m_decorated;
    }

    size_type size() const
    {
        return m_size;
    }

    friend bool operator==(const block_ptr& p1, const block_ptr& p2)
    {
        return p1.m_decorated == p2.m_decorated && p1.m_size == p2.m_size;
    }

    friend bool operator!=(const pointer_decorator& p1, const pointer_decorator& p2)
    {
        return !(p1 == p2);
    }
private:
    size_type m_size;
};

}}

#endif //ALLOCPP_BLOCK_PTR_H
