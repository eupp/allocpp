#ifndef ALLOCATION_MECHANISM_HPP
#define ALLOCATION_MECHANISM_HPP

#include <new>

#include "macro.hpp"
#include "details/allocation_request.hpp"
#include "details/allocation_response.hpp"
#include "details/deallocation_request.hpp"
#include "details/deallocation_response.hpp"

namespace alloc_utility { namespace plc {

template <typename T, typename void_alloc_traits, typename base_policy>
class allocation_mechanism: public base_policy
{
public:

    DECLARE_VOID_ALLOC_TRAITS(void_alloc_traits)
    DECLARE_REBIND_BASE_ALLOC(allocation_mechanism, base_policy, T, alloc_traits, arbitrator_t)

    details::allocation_response allocate(const details::allocation_request& request)
    {


        return ::operator new(n * sizeof(value_type), std::nothrow);
    }


};


}}

#endif // ALLOCATION_MECHANISM_HPP
