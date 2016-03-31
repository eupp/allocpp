#include <liballocpp/types.hpp>
#include <liballocpp/concepts/concepts.hpp>
#include <liballocpp/plcs/null.hpp>

using namespace allocpp;
using namespace allocpp::plcs;
using namespace allocpp::concepts;

ALLOCPP_CONCEPT_ASSERT((StatelessAllocPolicy<null<byte*>>));
