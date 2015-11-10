#ifndef ALLOCATION_MECHANISM_HPP
#define ALLOCATION_MECHANISM_HPP

#include "macro_traits.hpp"
#include "macro_alloc.hpp"
#include "ptr/pointer_factory.hpp"

namespace alloc_utility { namespace plc {

template <typename mechanism, typename void_alloc_traits, typename base_policy>
class allocation_mechanism: public base_policy
{
public:
    typedef mechanism mechanism_t;

    static_assert(void_alloc_traits_equality<void_alloc_traits, typename mechanism::traits_t>::value,
                  "Allocation mechanism should be compatible with traits type");

    DECLARE_VOID_ALLOC_TRAITS(void_alloc_traits)
    DECLARE_ALLOC_REQUEST_RESPONSE(void_alloc_traits)
    DECLARE_REBIND_ALLOC(allocation_mechanism, mechanism)

    typedef std::false_type propagate_on_container_copy_assignment;
    typedef std::false_type propagate_on_container_move_assignment;
    typedef std::false_type propagate_on_container_swap;

    allocation_mechanism() = default;
    allocation_mechanism(const allocation_mechanism&) = default;
    allocation_mechanism(allocation_mechanism&&) = default;

    allocation_mechanism& operator=(const allocation_mechanism&) = default;
    allocation_mechanism& operator=(allocation_mechanism&&) = default;

    void swap(allocation_mechanism&) noexcept
    {}

    allocation_response_t allocate(const allocation_request_t& request) noexcept
    {
        void* raw_ptr = mechanism::allocate(request);
        void_pointer ptr = ptr::pointer_factory<void_alloc_traits>::create(raw_ptr, request);
        return allocation_response_t(ptr, request.size());
    }

    deallocation_response_t deallocate(const deallocation_request_t& request) noexcept
    {
        void* raw_ptr = ptr::pointer_factory<void_alloc_traits>::destroy(request);
        bool succ = mechanism::deallocate(raw_ptr);
        return deallocation_response_t(succ);
    }


};


}}

#endif // ALLOCATION_MECHANISM_HPP
