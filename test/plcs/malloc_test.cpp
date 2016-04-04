#include <gtest/gtest.h>

#include <liballocpp/types.hpp>
#include <liballocpp/concepts/concepts.hpp>
#include <liballocpp/plcs/malloc.hpp>

using namespace allocpp;
using namespace allocpp::concepts;

ALLOCPP_CONCEPT_ASSERT((StatelessAllocPolicy<plcs::malloc>));

TEST(malloc_test, test_malloc)
{
    plcs::malloc malloc_plc;


}

