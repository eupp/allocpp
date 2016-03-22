#ifndef ALLOCPP_POINTER_TRAITS_HPP
#define ALLOCPP_POINTER_TRAITS_HPP

#include <memory>
#include <type_traits>
#include <boost/tti/member_type.hpp>
#include <boost/type_traits/conditional.hpp>

#include <liballocpp/concepts/pointer_concept_tag.hpp>

namespace allocpp { namespace utils {

template <typename Ptr>
class pointer_traits : public std::pointer_traits<Ptr>
{
    typedef std::pointer_traits<Ptr> traits;
    typedef typename std::add_lvalue_reference<typename traits::element_type>::type default_reference_type;

    BOOST_TTI_MEMBER_TYPE(reference)

    template <typename X>
    struct concept_tag_helper { typedef typename X::concept_tag concept_tag; };

    template <typename X>
    struct concept_tag_helper<X*> { typedef ::allocpp::concepts::random_access_ptr_tag concept_tag; };
public:
    using traits::element_type;
    typedef typename traits::element_type value_type;

    using traits::difference_type;
    typedef typename std::make_unsigned<typename traits::difference_type>::type size_type;

    using traits::pointer;
    typedef typename member_type_reference<Ptr, default_reference_type>::type reference;

    using traits::rebind;

    typedef typename concept_tag_helper<Ptr>::concept_tag concept_tag;
};

}}

#endif //ALLOCPP_POINTER_TRAITS_HPP
