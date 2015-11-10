#ifndef MALLOC_MECHANISM_HPP
#define MALLOC_MECHANISM_HPP

#include <cstdlib>

#include "macro_traits.hpp"
#include "allocation_traits.hpp"
#include "allocation_mechanism.hpp"

namespace alloc_utility { namespace plc {

class malloc_mechanism
{
public:

    typedef allocation_void_traits<void*> traits_t;

    DECLARE_ALLOC_REQUEST_RESPONSE(traits_t)

    static void* allocate(const allocation_request_t& request) noexcept
    {
        return malloc(request.size());
    }

    static bool deallocate(void* ptr) noexcept
    {
        free(ptr);
        return true;
    }

};

template <typename void_alloc_traits, typename base_policy>
using malloc_allocation = allocation_mechanism<malloc_mechanism, void_alloc_traits, base_policy>;

}}

#endif // MALLOC_MECHANISM_HPP
