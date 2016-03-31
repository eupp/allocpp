#ifndef ALLOCPP_CONCEPT_HPP
#define ALLOCPP_CONCEPT_HPP

#include <memory>
#include <utility>
#include <boost/concept_check.hpp>

#include <liballocpp/alloc_messaging.hpp>
#include <liballocpp/ptrs/block_ptr.hpp>
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
        reference_type ref = *p;    // require dereferencing
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
        reference_type ref = p[i];      // require subscripting
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

template <typename T>
class MemoryLayout
{
    typedef typename T::pointer_type pointer_type;
    typedef typename T::size_type size_type;
    typedef alloc_request<pointer_type> alloc_request_type;
    typedef alloc_response<pointer_type> alloc_response_type;
    typedef dealloc_request<pointer_type> dealloc_request_type;
    typedef dealloc_response<pointer_type> dealloc_response_type;
public:
    ALLOCPP_CONCEPT_USAGE(MemoryLayout)
    {
        T layout(alloc_req);                                            // require construction from alloc_request

        alloc_response_type alloc_resp = layout.allocate(alloc_req);
        dealloc_response_type dealloc_resp = layout.deallocate(dealloc_req);

        bool b;
        b = layout.is_available(s);
        b = layout.owns(p);
        b = layout.empty();
    }
private:
    pointer_type p;
    size_type s;
    alloc_request_type alloc_req;
    dealloc_request_type dealloc_req;
};

template <typename T>
class StaticMemoryRegion
{
    typedef typename T::pointer_type pointer_type;
    typedef typename T::size_type size_type;
    typedef typename T::concept_tag concept_tag;
public:
    ALLOCPP_CONCEPT_USAGE(StaticMemoryRegion)
    {
        T region;                           // require default construction

        pointer_type p = region.memory();
        size_type s = region.size();

        p = region.begin();
        p = region.end();

        bool b = region.owns(p);
        ALLOCPP_UNUSED(b);
    }
};

template <typename T>
class DynamicMemoryRegion
{
    typedef typename T::pointer_type pointer_type;
    typedef typename T::size_type size_type;
    typedef typename T::concept_tag concept_tag;
    typedef ptrs::block_ptr<pointer_type> block_ptr;
public:
    ALLOCPP_CONCEPT_USAGE(DynamicMemoryRegion)
    {
        T region(block);                           // require construction from block_ptr

        pointer_type p = region.memory();
        size_type s = region.size();

        p = region.begin();
        p = region.end();

        bool b = region.owns(p);
        ALLOCPP_UNUSED(b);
    }
private:
    block_ptr block;
};

}}

#endif //ALLOCPP_CONCEPT_HPP
