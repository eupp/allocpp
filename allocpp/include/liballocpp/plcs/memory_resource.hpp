#ifndef ALLOCPP_MEMORY_RESOURCE_HPP
#define ALLOCPP_MEMORY_RESOURCE_HPP

#include <liballocpp/concepts/concepts.hpp>

namespace allocpp { namespace plcs {

template <typename Layout, typename Region, typename UpstreamAlloc>
class memory_resource : private Region, private Layout, private UpstreamAlloc
{
public:
    ALLOCPP_CONCEPT_ASSERT((concepts::MemoryLayout<Layout>));
    ALLOCPP_CONCEPT_ASSERT((concepts::MemoryRegion<Region>));

private:
};

}}

#endif //ALLOCPP_MEMORY_RESOURCE_HPP
