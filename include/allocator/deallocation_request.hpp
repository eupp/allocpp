#ifndef DEALLOCATION_REQUEST_HPP
#define DEALLOCATION_REQUEST_HPP

#include "macro_traits.hpp"

namespace alloc_utility {

template <typename void_alloc_traits>
class deallocation_request
{
public:

    DECLARE_VOID_ALLOC_TRAITS(void_alloc_traits)

    deallocation_request(const void_pointer& ptr = nullptr)
        : m_ptr(ptr)
    {}

    void_pointer pointer() const
    {
        return m_ptr;
    }

    void set_pointer(const void_pointer& ptr)
    {
        m_ptr = ptr;
    }

private:
    void_pointer m_ptr;
};

}

#endif // DEALLOCATION_REQUEST_HPP
