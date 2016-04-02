#include <liballocpp/types.hpp>
#include <liballocpp/concepts/concepts.hpp>
#include <liballocpp/plcs/static_region.hpp>
#include <liballocpp/plcs/dynamic_region.hpp>

using namespace allocpp;
using namespace allocpp::plcs;
using namespace allocpp::concepts;

ALLOCPP_CONCEPT_ASSERT((StaticMemoryRegion<static_region<64>>));
ALLOCPP_CONCEPT_ASSERT((DynamicMemoryRegion<dynamic_region<byte*>>));

ALLOCPP_CONCEPT_ASSERT((MemoryRegion<static_region<64>>));
ALLOCPP_CONCEPT_ASSERT((MemoryRegion<dynamic_region<byte*>>));