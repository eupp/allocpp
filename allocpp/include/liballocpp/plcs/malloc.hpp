#ifndef ALLOCPP_MALLOC_HPP
#define ALLOCPP_MALLOC_HPP

#include <cstddef>
#include <limits>

#include <liballocpp/types.hpp>
#include <liballocpp/alloc_messaging.hpp>
#include <liballocpp/concepts/tags.hpp>
#include <liballocpp/utils/util.hpp>

namespace allocpp { namespace plcs {

class malloc
{
public:
    typedef byte* pointer_type;
    typedef concepts::stateless_alloc_policy_tag concept_tag;

    static constexpr size_t max_size()
    {
        return std::numeric_limits<size_t>::max();
    }

    static constexpr size_t max_alignment()
    {
        return alignof(std::max_align_t);
    }

    malloc() = default;
    malloc(const malloc&) = default;
    malloc(malloc&&) = default;

    alloc_response<byte*> allocate(const alloc_request<byte*>& req);
    dealloc_response<byte*> deallocate(const dealloc_request<byte*>& req);

    ownership owns(byte* p);
};

}}

#endif //ALLOCPP_MALLOC_HPP
