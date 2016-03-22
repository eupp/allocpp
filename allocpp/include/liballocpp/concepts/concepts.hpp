#ifndef ALLOCPP_CONCEPT_HPP
#define ALLOCPP_CONCEPT_HPP

#include <memory>
#include <utility>
#include <boost/concept_check.hpp>

#include <liballocpp/utils/pointer.hpp>

template <typename... Ts>
void ALLOCPP_UNUSED(Ts&&...) {}

#define ALLOCPP_CONCEPT_USAGE BOOST_CONCEPT_USAGE
#define ALLOCPP_CONCEPT_ASSERT BOOST_CONCEPT_ASSERT

namespace allocpp { namespace concepts {

template <typename T>
class NullablePtr
    : boost::DefaultConstructible<T>
    , boost::CopyConstructible<T>
    , boost::Assignable<T>
    , boost::EqualityComparable<T>
    , boost::Convertible<T, bool>
{
public:
    ALLOCPP_CONCEPT_USAGE(NullablePtr)
    {
        T np(nullptr);          // require construction from nullptr

        using std::swap;
        swap(p, np);            // swappable

        bool b1 = p == nullptr;
        bool b2 = nullptr == p; // require equality comparison with nullptr
        ALLOCPP_UNUSED(b1, b2);
    }
private:
    T p;
};

template <typename T>
class ObjectPtr
    : NullablePtr<T>
{
    typedef typename utils::pointer_traits<T>::element_type element_type;
    typedef typename utils::pointer_traits<T>::reference reference_type;
public:
    ALLOCPP_CONCEPT_USAGE(ObjectPtr)
    {
        reference_type ref = *p;
        *p = e;
        ALLOCPP_UNUSED(ref);
    }
private:
    T p;
    element_type e;
};

template <typename T>
class ArrayPtr
    : ObjectPtr<T>
{
    typedef typename utils::pointer_traits<T>::element_type element_type;
    typedef typename utils::pointer_traits<T>::size_type size_type;
    typedef typename utils::pointer_traits<T>::reference reference_type;
public:
    ALLOCPP_CONCEPT_USAGE(ArrayPtr)
    {
        reference_type ref = p[i];
        p[i] = e;
        ALLOCPP_UNUSED(ref);
    }
private:
    T p;
    size_type i;
    element_type e;
};

template <typename T>
class RandomAccessPtr
    : ArrayPtr<T>
    , boost::RandomAccessIterator<T>
{};

}}

#endif //ALLOCPP_CONCEPT_HPP
