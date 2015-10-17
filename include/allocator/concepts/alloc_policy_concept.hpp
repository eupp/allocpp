#ifndef ALLOC_POLICY_CONCEPT_HPP
#define ALLOC_POLICY_CONCEPT_HPP

#include <type_traits>

#include "details/alloc_type_traits.hpp"
#include "pointer_concepts.hpp"

namespace alloc_utility
{

namespace concepts
{

template <typename T>
struct is_alloc_policy: public std::integral_constant<bool,
           std::is_default_constructible<T>::value
        && std::is_copy_constructible<T>::value
        && std::is_move_constructible<T>::value
        && details::is_constructible_from_rebind<T>::value
        && std::is_destructible<T>::value
        && std::is_nothrow_copy_assignable<T>::value
        && std::is_nothrow_move_assignable<T>::value
        && details::is_nothrow_swappable<T, T>::value
//        && is_
        >
{};

} // namespace concepts

} // namespace alloc_utility

#endif // ALLOC_POLICY_CONCEPT_HPP
