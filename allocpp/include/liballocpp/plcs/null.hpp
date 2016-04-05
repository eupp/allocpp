#ifndef ALLOCPP_NULL_HPP
#define ALLOCPP_NULL_HPP

#include <liballocpp/concepts/tags.hpp>
#include <liballocpp/alloc_messaging.hpp>

namespace allocpp { namespace plcs {

template <typename Ptr>
class null
{
public:
    typedef Ptr pointer_type;
    typedef concepts::stateless_alloc_policy_tag concept_tag;

    static constexpr size_t max_size()
    {
        return 0;
    }

    static constexpr size_t max_alignment()
    {
        return 0;
    }

    null() = default;
    null(const null&) = default;

    alloc_response<pointer_type> allocate(const alloc_request<pointer_type>&)
    {
        return alloc_response<pointer_type>();
    }

    dealloc_response<pointer_type> deallocate(const dealloc_request<pointer_type>&)
    {
        return dealloc_response<pointer_type>();
    }

    ownership owns(pointer_type)
    {
        return ownership::UNDEFINED;
    }
};

}}

#endif //ALLOCPP_NULL_HPP
