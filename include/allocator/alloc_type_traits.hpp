#ifndef ALLOC_TYPE_TRAITS_HPP
#define ALLOC_TYPE_TRAITS_HPP

#include <type_traits>

namespace alloc_utility
{

namespace details
{

template<typename...>
struct voider
{
    using type = void;
};

template <typename... Ts>
using void_t = typename voider<Ts...>::type;

struct placeholder
{};

} // namespace details

template <typename T, typename = void>
struct has_rebind: std::false_type
{};

template <typename T>
struct has_rebind<T, details::void_t<typename T::template rebind<details::placeholder>>>: std::true_type
{};

template <typename T, typename = void>
struct has_equal_to_op: std::false_type
{};

template <typename T>
struct has_equal_to_op<
        T,
        details::void_t<decltype(std::declval<T>() == std::declval<T>())>
    >: std::is_same<bool, decltype(std::declval<T>() == std::declval<T>())>
{};

template <typename T, typename = void>
struct has_not_equal_to_op: std::false_type
{};

template <typename T>
struct has_not_equal_to_op<
        T,
        details::void_t<decltype(std::declval<T>() != std::declval<T>())>
    >: std::is_same<bool, decltype(std::declval<T>() != std::declval<T>())>
{};

} // namespace alloc_utility

#endif // ALLOC_TYPE_TRAITS_HPP
