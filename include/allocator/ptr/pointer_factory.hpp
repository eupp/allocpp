#ifndef POINTER_FACTORY_HPP
#define POINTER_FACTORY_HPP

#include "macro.hpp"
#include "allocation_response.hpp"
#include "deallocation_request.hpp"

namespace alloc_utility { namespace ptr {

template <typename void_alloc_traits>
struct pointer_factory
{
    DECLARE_VOID_ALLOC_TRAITS(void_alloc_traits)

    void_pointer create(const allocation_response& response);
    void destroy(const deallocation_request& request);
};

} }

#endif // POINTER_FACTORY_HPP
