#ifndef ALLOCATION_REQUEST_HPP
#define ALLOCATION_REQUEST_HPP

#include "macro.hpp"

namespace alloc_utility {

template <typename void_alloc_traits>
class allocation_request
{
public:

    DECLARE_VOID_ALLOC_TRAITS(void_alloc_traits)

    allocation_request()
        : m_obj_count(0)
        , m_obj_size(0)
        , m_alignment(0)
        , m_padding(0)
        , m_hint(nullptr)
    {}

    allocation_request(size_type obj_size, size_type obj_count, const const_void_pointer& hint = nullptr)
        : m_obj_count(obj_count)
        , m_obj_size(obj_size)
        , m_alignment(0)
        , m_padding(0)
        , m_hint(hint)
    {}

    size_type size() const
    {
        return (m_obj_size + m_padding) * m_obj_count;
    }

    size_type obj_count() const
    {
        return m_obj_count;
    }

    void set_obj_count(size_type obj_count)
    {
        m_obj_count = obj_count;
    }

    size_type obj_size() const
    {
        return m_obj_size;
    }

    void set_obj_size(size_type obj_size)
    {
        m_obj_size = obj_size;
    }

    size_type alignment() const
    {
        return m_alignment;
    }

    void set_alignment(size_type alignment)
    {
        m_alignment = alignment;
    }

    size_type padding() const
    {
        return m_padding;
    }

    void set_padding(size_type padding)
    {
        m_padding = padding;
    }

    const_void_pointer hint() const
    {
        return m_hint;
    }

    void set_hint(const const_void_pointer& hint)
    {
        m_hint = hint;
    }

private:
    size_type m_obj_count;
    size_type m_obj_size;
    size_type m_alignment;
    size_type m_padding;
    const_void_pointer m_hint;
};

}

#endif // ALLOCATION_REQUEST_HPP
