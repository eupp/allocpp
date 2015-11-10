#ifndef MACRO_ALLOC_HPP
#define MACRO_ALLOC_HPP

#include "allocation_request.hpp"
#include "allocation_response.hpp"
#include "deallocation_request.hpp"
#include "deallocation_response.hpp"

#define DECLARE_ALLOC_REQUEST_RESPONSE(void_alloc_traits)                       \
    typedef allocation_request<void_alloc_traits> allocation_request_t;         \
    typedef allocation_response<void_alloc_traits> allocation_response_t;       \
    typedef deallocation_request<void_alloc_traits> deallocation_request_t;     \
    typedef deallocation_response<void_alloc_traits> deallocation_response_t;

#endif // MACRO_ALLOC_HPP
