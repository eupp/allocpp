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
struct has_rebind: public std::false_type
{};

template <typename T>
struct has_rebind<T, details::void_t<typename T::template rebind<details::placeholder>>>: public std::true_type
{};

template <typename T, typename = void>
struct has_equal_to_op: public std::false_type
{};

template <typename T>
struct has_equal_to_op<
        T,
        details::void_t<decltype(std::declval<T>() == std::declval<T>())>
    >: public std::is_same<bool, decltype(std::declval<T>() == std::declval<T>())>
{};

template <typename T, typename = void>
struct has_not_equal_to_op: public std::false_type
{};

template <typename T>
struct has_not_equal_to_op<
        T,
        details::void_t<decltype(std::declval<T>() != std::declval<T>())>
    >: public std::is_same<bool, decltype(std::declval<T>() != std::declval<T>())>
{};

template <typename T, typename = void>
struct enable_propagate_on_copy: public std::false_type
{};

template <typename T>
struct enable_propagate_on_copy<T, details::void_t<typename T::propagate_on_container_copy_assignment>>:
        public T::propagate_on_container_copy_assignment
{};

template <typename T, typename = void>
struct enable_propagate_on_move: public std::false_type
{};

template <typename T>
struct enable_propagate_on_move<T, details::void_t<typename T::propagate_on_container_move_assignment>>:
        public T::propagate_on_container_move_assignment
{};

template <typename T, typename = void>
struct enable_propagate_on_swap: public std::false_type
{};

template <typename T>
struct enable_propagate_on_swap<T, details::void_t<typename T::propagate_on_container_swap>>:
        public T::propagate_on_container_swap
{};

} // namespace alloc_utility

#endif // ALLOC_TYPE_TRAITS_HPP
