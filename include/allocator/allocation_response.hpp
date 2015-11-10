#ifndef ALLOCATION_RESPONSE_HPP
#define ALLOCATION_RESPONSE_HPP

#include "macro_traits.hpp"

namespace alloc_utility {

template <typename void_alloc_traits>
class allocation_response
{
public:

    DECLARE_VOID_ALLOC_TRAITS(void_alloc_traits)

    allocation_response()
        : m_ptr(nullptr)
        , m_size(0)
        , m_alignment(0)
        , m_hint_used(false)
    {}

    allocation_response(const void_pointer& ptr, size_type size)
        : m_ptr(ptr)
        , m_size(size)
        , m_alignment(0)
        , m_hint_used(false)
    {}

    void_pointer pointer() const
    {
        return m_ptr;
    }

    void set_pointer(const void_pointer& ptr)
    {
        m_ptr = ptr;
    }

    size_type size() const
    {
        return m_size;
    }

    void set_size(size_type size)
    {
        m_size = size;
    }

    size_type alignment() const
    {
        return m_alignment;
    }

    void set_alignment(size_type alignment)
    {
        m_alignment = alignment;
    }

    bool hint_used() const
    {
        return m_hint_used;
    }

    void set_hint_used(bool hint_used)
    {
        m_hint_used = hint_used;
    }

private:
    void_pointer m_ptr;
    size_type m_size;
    size_type m_alignment;
    bool m_hint_used;
};

}

#endif // ALLOCATION_RESPONSE_HPP



