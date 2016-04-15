#ifndef ALLOCPP_CONCEPT_HPP
#define ALLOCPP_CONCEPT_HPP

#include <memory>
#include <utility>
#include <boost/concept_check.hpp>

#include <liballocpp/alloc_messaging.hpp>
#include <liballocpp/concepts/tags.hpp>
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

namespace details {
void is_stateless_plc(const stateless_alloc_policy_tag&);
void is_stateful_plc(const stateful_alloc_policy_tag&);
void is_memory_layout_plc(const memory_layout_policy_tag&);

template <typename T>
void same_type(const T&, const T&);
}

template <typename T>
class BaseAllocPolicy
{
    typedef typename T::pointer_type pointer_type;
    typedef typename utils::pointer_traits<pointer_type>::size_type size_type;
    typedef alloc_request<pointer_type> alloc_request_type;
    typedef alloc_response<pointer_type> alloc_response_type;
    typedef dealloc_request<pointer_type> dealloc_request_type;
    typedef dealloc_response<pointer_type> dealloc_response_type;
public:
    ALLOCPP_CONCEPT_USAGE(BaseAllocPolicy)
    {
        alloc_response_type alloc_resp = alloc.allocate(alloc_req);
        dealloc_response_type dealloc_resp = alloc.deallocate(dealloc_req);

        size_type max_size = T::max_size();
        size_type max_alignment = T::max_alignment();

        ownership owns = alloc.owns(p);
        ALLOCPP_UNUSED(owns);
    }
private:
    T alloc;
    pointer_type p;
    alloc_request_type alloc_req;
    dealloc_request_type dealloc_req;
};

template <typename T>
class StatelessAllocPolicy
    : BaseAllocPolicy<T>
{
    typedef typename T::concept_tag concept_tag;
public:
    ALLOCPP_CONCEPT_USAGE(StatelessAllocPolicy)
    {
        T alloc;                                                // require default construction
        T alloc_(alloc);                                        // require copy construction

        details::is_stateless_plc(concept_tag());
    }
};

template <typename T>
class StatefulAllocPolicy
    : BaseAllocPolicy<T>
{
    typedef typename T::pointer_type pointer_type;
    typedef typename T::concept_tag concept_tag;
    typedef alloc_request<pointer_type> alloc_request_type;
    typedef alloc_response<pointer_type> alloc_response_type;
    typedef dealloc_request<pointer_type> dealloc_request_type;
public:
    ALLOCPP_CONCEPT_USAGE(StatefulAllocPolicy)
    {
        bool b;
        b = alloc.is_feasible(alloc_req);
        b = alloc.is_feasible(dealloc_req);
        b = alloc.empty();                      // checks whether allocator is empty,
                                                // i.e. there is no chunks allocated by it
        ALLOCPP_UNUSED(b);

        details::is_stateful_plc(concept_tag());
    }
private:
    T alloc;
    alloc_request_type alloc_req;
    dealloc_request_type dealloc_req;
};

template <typename T>
class MemoryLayoutPolicy
    : StatefulAllocPolicy<T>
{
    typedef typename T::pointer_type pointer_type;
    typedef typename utils::pointer_traits<pointer_type>::size_type size_type;
    typedef alloc_request<pointer_type> alloc_request_type;
    typedef typename T::concept_tag concept_tag;

    typedef typename T::internal_pointer_type internal_pointer_type;
    typedef typename utils::pointer_traits<internal_pointer_type>::size_type internal_size_type;
    typedef ptrs::block_ptr<internal_pointer_type> block_ptr_type;
public:
    ALLOCPP_CONCEPT_USAGE(MemoryLayoutPolicy)
    {
        T layout(block, alloc_req);                 // require construction from block_ptr and alloc_request

        internal_size_type blk_size = T::block_size(alloc_req);
        block_ptr_type blk = layout.block();

        ALLOCPP_UNUSED(blk, blk_size);

        details::is_memory_layout_plc(concept_tag());
    }
private:
    block_ptr_type block;
    alloc_request_type alloc_req;
};

}}

#endif //ALLOCPP_CONCEPT_HPP
