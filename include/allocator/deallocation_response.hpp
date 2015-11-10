#ifndef DEALLOCATION_RESPONSE_HPP
#define DEALLOCATION_RESPONSE_HPP

#include "macro_traits.hpp"

namespace alloc_utility {

template <typename void_alloc_traits>
class deallocation_response
{
public:

    DECLARE_VOID_ALLOC_TRAITS(void_alloc_traits)

    deallocation_response(bool succeeded)
        : m_success(succeeded)
    {}

    bool succeeded() const
    {
        return m_success;
    }

    void set_succeeded(bool succ)
    {
        m_success = succ;
    }

private:
    bool m_success;
};

}


#endif // DEALLOCATION_RESPONSE_HPP
