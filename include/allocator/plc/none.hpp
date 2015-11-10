#ifndef NONE_POLICY_HPP
#define NONE_POLICY_HPP

#include "macro_traits.hpp"
#include "macro_alloc.hpp"

namespace alloc_utility
{

template <typename void_alloc_traits>
class none
{
public:

    DECLARE_VOID_ALLOC_TRAITS(void_alloc_traits)
    DECLARE_ALLOC_REQUEST_RESPONSE(void_alloc_traits)

    typedef std::false_type propagate_on_container_copy_assignment;
    typedef std::false_type propagate_on_container_move_assignment;
    typedef std::false_type propagate_on_container_swap;

    none() = default;
    none(const none& other) = default;
    none(none&&) = default;

    none& operator=(const none&) = default;
    none& operator=(none&&) = default;

    void swap(none&) noexcept
    {}

    allocation_response_t allocate(const allocation_request_t&) noexcept
    {
        return allocation_response_t();
    }

    deallocation_response_t deallocate(const deallocation_request_t&) noexcept
    {
        return deallocation_response_t(false);
    }
};

}

#endif // NONE_POLICY_HPP
