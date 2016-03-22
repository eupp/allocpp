#include <gtest/gtest.h>

#include <boost/concept_check.hpp>

#include <liballocpp/concepts/concepts.hpp>

using namespace allocpp::concepts;

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
    bool b = std::is_same<
            allocpp::concepts::random_access_ptr_tag,
            typename allocpp::utils::pointer_traits<int*>::concept_tag
        >::value;
    ASSERT_TRUE(b);
}