#ifndef POINTER_FACTORY_HPP
#define POINTER_FACTORY_HPP

#include "macro_traits.hpp"
#include "allocation_traits.hpp"
#include "allocation_request.hpp"
#include "deallocation_request.hpp"

namespace alloc_utility { namespace ptr {

template <typename void_alloc_traits>
struct pointer_factory
{
    typedef void_alloc_traits traits_t;

    DECLARE_VOID_ALLOC_TRAITS(void_alloc_traits)

    typedef allocation_response<traits_t> allocation_request_t;
    typedef deallocation_request<traits_t> deallocation_request_t;

    static void_pointer create(void* ptr, const allocation_request_t& request);
    static void* destroy(const deallocation_request_t& request);
};

template <typename T>
struct pointer_factory<allocation_void_traits<T*>>
{
    typedef allocation_void_traits<T*> traits_t;

    DECLARE_VOID_ALLOC_TRAITS(traits_t)

    typedef allocation_request<traits_t> allocation_request_t;
    typedef deallocation_request<traits_t> deallocation_request_t;

    static void_pointer create(void* ptr, const allocation_request_t&)
    {
        return ptr;
    }

    static void* destroy(const deallocation_request_t& request)
    {
        return request.pointer();
    }
};

} }

#endif // POINTER_FACTORY_HPP
