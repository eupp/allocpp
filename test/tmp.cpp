#include <gtest/gtest.h>

#include <boost/concept_check.hpp>

#include "liballocpp/concept/concept.hpp"

using namespace allocpp::concept;

#define MY_ASSERT_EQ ASSERT_EQ

template <typename T>
T test_nullable_ptr(T t)
{
    BOOST_CONCEPT_ASSERT((NullablePtr<T>));
    return t;
}

template <typename T>
T test_object_ptr(T t)
{
    BOOST_CONCEPT_ASSERT((ObjectPtr<T>));
    return t;
}

TEST(tmp, tmp)
{
    int v = 0;
    MY_ASSERT_EQ(nullptr, test_nullable_ptr(nullptr));
    MY_ASSERT_EQ(&v, test_object_ptr(&v));
}