#ifndef ALLOCPP_POINTER_TRAITS_HPP
#define ALLOCPP_POINTER_TRAITS_HPP

#include <memory>
#include <type_traits>
#include <boost/tti/member_type.hpp>

namespace allocpp { namespace util {

template <typename Ptr>
class pointer_traits: public std::pointer_traits<Ptr>
{
    typedef std::pointer_traits<Ptr> traits;
    typedef typename std::add_lvalue_reference<typename traits::element_type>::type default_reference_type;

    BOOST_TTI_MEMBER_TYPE(reference_type)
public:
    using traits::pointer;
    using traits::element_type;
    using traits::difference_type;
    using traits::rebind;

    typedef typename member_type_reference_type<Ptr, default_reference_type>::type reference_type;
};

}}

#endif //ALLOCPP_POINTER_TRAITS_HPP
