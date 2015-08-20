#ifndef POINTER_CONCEPTS_HPP
#define POINTER_CONCEPTS_HPP

#include <type_traits>

#include "details/alloc_type_traits.hpp"

namespace alloc_utility
{

namespace pointer_concepts
{

template <typename T>
struct single_object_ptr_trait: public std::integral_constant<bool,
           std::is_default_constructible<T>::value
        && std::is_constructible<T, std::nullptr_t>::value
        && std::is_copy_constructible<T>::value
        && std::is_copy_assignable<T>::value
        && std::is_assignable<T, std::nullptr_t>::value
        && std::is_destructible<T>::value
        && details::is_equality_comparable<T, T>::value
        && details::is_equality_comparable<T, std::nullptr_t>::value
        && details::is_contextual_convertible_to_bool<T>::value
        && details::has_dereference_operator<T>::value
        && details::is_swappable<T, T>::value
        >
{};




} // pointer_concepts


} // alloc_utility

#endif // POINTER_CONCEPTS_HPP
