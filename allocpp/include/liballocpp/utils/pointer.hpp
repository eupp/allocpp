#ifndef ALLOCPP_POINTER_H
#define ALLOCPP_POINTER_H

#include <memory>
#include <type_traits>
#include <boost/tti/member_type.hpp>

#include <liballocpp/concepts/tags.hpp>

namespace allocpp { namespace utils {

template <typename Ptr>
using is_raw_ptr = typename std::is_same<Ptr, typename std::pointer_traits<Ptr>::element_type*>::type;

template <typename Ptr, typename = typename std::enable_if<is_raw_ptr<Ptr>::value>::type>
Ptr get_ptr(Ptr p)
{
    return p;
}

template <typename Ptr, typename = typename std::enable_if<!is_raw_ptr<Ptr>::value>::type>
auto get_ptr(const Ptr& p)
    -> typename std::result_of<decltype(&Ptr::get)(Ptr)>::type
{
    return p.get();
}

template <typename Ptr>
class pointer_traits : public std::pointer_traits<Ptr>
{
    typedef std::pointer_traits<Ptr> traits;
    typedef typename std::add_lvalue_reference<typename traits::element_type>::type default_reference;
    typedef typename std::add_lvalue_reference<
            typename std::add_const<typename traits::element_type>::type
        >::type default_const_reference;

    BOOST_TTI_MEMBER_TYPE(reference)
    BOOST_TTI_MEMBER_TYPE(const_reference)

    template <typename X>
    struct concept_tag_helper { typedef typename X::concept_tag concept_tag; };

    template <typename X>
    struct concept_tag_helper<X*> { typedef concepts::random_access_ptr_tag concept_tag; };
public:
    using traits::element_type;
    typedef typename traits::element_type value_type;

    using traits::difference_type;
    typedef typename std::make_unsigned<typename traits::difference_type>::type size_type;

    using traits::pointer;
    typedef typename member_type_reference<Ptr, default_reference>::type reference;
    typedef typename member_type_const_reference<Ptr, default_const_reference>::type const_reference;

    using traits::rebind;

    typedef typename concept_tag_helper<Ptr>::concept_tag concept_tag;
};

}}

#endif //ALLOCPP_POINTER_H
