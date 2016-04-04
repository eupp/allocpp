#ifndef ALLOCPP_MALLOC_HPP
#define ALLOCPP_MALLOC_HPP

#include <liballocpp/types.hpp>
#include <liballocpp/alloc_messaging.hpp>
#include <liballocpp/concepts/tags.hpp>
#include <liballocpp/utils/util.hpp>

namespace allocpp { namespace plcs {

class malloc : private utils::nonassignable
{
public:
    typedef byte* pointer_type;
    typedef concepts::stateless_alloc_policy_tag concept_tag;

    malloc() = default;
    malloc(const malloc&) = default;
    malloc(malloc&&) = default;

    alloc_response<byte*> allocate(const alloc_request<byte*>& req);
    dealloc_response<byte*> deallocate(const dealloc_request<byte*>& req);

    ownership owns(byte* p);
};

}}

#endif //ALLOCPP_MALLOC_HPP
