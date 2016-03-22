#include <gtest/gtest.h>

#include <type_traits>

#include <liballocpp/concepts/concepts.hpp>
#include <liballocpp/concepts/pointer_concept_tag.hpp>
#include <liballocpp/ptrs/pointer_decorator.hpp>

using namespace allocpp::ptrs;
using namespace allocpp::concepts;

namespace {

typedef pointer_decorator<int*, object_ptr_tag> decorated1;
typedef pointer_decorator<decorated1, object_ptr_tag> decorated2;
typedef pointer_decorator<int*, array_ptr_tag> decorated3;

}

ALLOCPP_CONCEPT_ASSERT((ObjectPtr<decorated1>));
ALLOCPP_CONCEPT_ASSERT((ObjectPtr<decorated2>));
ALLOCPP_CONCEPT_ASSERT((ArrayPtr<decorated3>));

TEST(pointer_decorator_test, test_types)
{
    bool b1 = std::is_same<int*, decorated1::decorated_type>::value;
    ASSERT_TRUE(b1);

    bool b2 = std::is_same<int*, decorated1::innermost_type>::value;
    ASSERT_TRUE(b2);

    bool b3 = std::is_same<decorated1, decorated2::decorated_type>::value;
    ASSERT_TRUE(b3);

    bool b4 = std::is_same<int*, decorated2::innermost_type>::value;
    ASSERT_TRUE(b4);
}

TEST(pointer_decorator_test, test_default_constructor)
{
    decorated1 p;
    ASSERT_EQ(nullptr, p.get());
}

TEST(pointer_decorator_test, test_constructor_decorated)
{
    int val = 0;
    decorated1 p1(&val);
    ASSERT_EQ(&val, p1.get());

    decorated2 p2(p1);
    ASSERT_EQ(&val, p2.get());
}

TEST(pointer_decorator_test, test_constructor_innermost)
{
    int val = 0;
    decorated2 p(&val);
    ASSERT_EQ(&val, p.get());
}

TEST(pointer_decorator_test, test_assign)
{
    int val = 0;
    decorated1 p1;
    decorated1 p2(&val);
    p1 = p2;
    ASSERT_EQ(&val, p1.get());
}

TEST(pointer_decorator_test, test_decorated_ptr)
{
    int val = 0;
    decorated1 p;
    p.decorated_ptr() = &val;
    ASSERT_EQ(&val, p.get());
}

TEST(pointer_decorator_test, test_bool_convert)
{
    int val = 0;
    decorated1 p1;
    decorated2 p2;
    decorated1 p3(&val);
    decorated2 p4(&val);

    ASSERT_FALSE(p1);
    ASSERT_FALSE(p2);
    ASSERT_TRUE(p3);
    ASSERT_TRUE(p4);
}

TEST(pointer_decorator_test, test_eq_cmp)
{
    int val = 0;
    decorated1 p1;
    decorated1 p2(&val);
    decorated1 p3(&val);
    decorated2 p4(&val);
    decorated2 p5(&val);

    ASSERT_EQ(nullptr, p1);
    ASSERT_NE(nullptr, p2);
    ASSERT_EQ(p2, p3);
    ASSERT_EQ(p4, p5);
}

TEST(pointer_decorator_test, test_dereference)
{
    int val = 0;
    decorated1 p1(&val);
    decorated2 p2(&val);

    ASSERT_EQ(0, *p1);
    ASSERT_EQ(0, *p2);

    *p1 = 1;
    ASSERT_EQ(1, val);

    *p2 = 2;
    ASSERT_EQ(2, val);
}

TEST(pointer_decorator_test, test_array_subscript)
{
    int val[2] = {0};
    decorated3 p(val);

    ASSERT_EQ(0, p[0]);
    ASSERT_EQ(0, p[1]);

    p[1] = 1;
    ASSERT_EQ(1, val[1]);

    p[1] = 2;
    ASSERT_EQ(2, val[1]);
}