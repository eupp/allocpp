#ifndef POINTER_CONCEPTS_HPP
#define POINTER_CONCEPTS_HPP

#include <memory>
#include <type_traits>

#include "details/alloc_type_traits.hpp"

namespace alloc_utility
{

namespace concepts
{

template <typename T>
struct is_nullable_ptr: public std::integral_constant<bool,
           std::is_default_constructible<T>::value
        && std::is_copy_constructible<T>::value
        && std::is_nothrow_constructible<T, std::nullptr_t>::value
        && std::is_copy_assignable<T>::value
        && std::is_destructible<T>::value
        && details::is_equality_comparable<T, T>::value
        && details::is_equality_comparable<T, std::nullptr_t>::value
        && details::is_contextual_convertible_to_bool<T>::value
        && details::is_swappable<T, T>::value
        >
{};

template <typename T>
struct is_single_object_ptr: public std::integral_constant<bool,
           is_nullable_ptr<T>::value
        && details::has_dereference_operator<T>::value
        >
{};

template <typename T>
struct is_array_ptr: public std::integral_constant<bool,
           is_single_object_ptr<T>::value
        && details::has_array_subscript_operator<T>::value
        >
{};

template <typename T>
struct is_random_access_ptr: public std::integral_constant<bool,
           is_array_ptr<T>::value

        && details::has_pre_increment_operator<T>::value
        && details::has_pre_decrement_operator<T>::value
        && details::has_post_increment_operator<T>::value
        && details::has_post_decrement_operator<T>::value

        && details::supports_addition<T, typename details::enable_difference_type<T>::type, T>::value
        && details::supports_addition<T, T, typename details::enable_difference_type<T>::type>::value

        && details::supports_substraction<T, T, typename details::enable_difference_type<T>::type>::value
        && details::supports_substraction<typename details::enable_difference_type<T>::type, T, T>::value

        && details::supports_add_assign<T, typename details::enable_difference_type<T>::type>::value
        && details::supports_sub_assign<T, typename details::enable_difference_type<T>::type>::value

        && details::is_less_than_comparable<T, T>::value
        && details::is_greater_than_comparable<T, T>::value
        && details::is_less_equal_comparable<T, T>::value
        && details::is_greater_equal_comparable<T, T>::value
        >
{};

} // namespace concepts


} // namespace alloc_utility

#endif // POINTER_CONCEPTS_HPP
