#ifndef ALLOCPP_STATIC_REGION_HPP
#define ALLOCPP_STATIC_REGION_HPP

#include <cstddef>

#include <liballocpp/types.hpp>
#include <liballocpp/concepts/tags.hpp>

namespace allocpp { namespace plcs {

template <size_t N>
class static_region
{
public:
    typedef byte* pointer_type;
    typedef size_t size_type;
    typedef concepts::static_region_tag concept_tag;

    pointer_type memory()
    {
        return mem;
    }

    size_type size() const
    {
        return N;
    }

    pointer_type begin()
    {
        return mem;
    }

    pointer_type end()
    {
        return mem + N;
    }

    bool owns(byte* p)
    {
        return (begin() <= p) && (p < end());
    }
private:
    byte mem[N];
};

}}

#endif //ALLOCPP_STATIC_REGION_HPP
