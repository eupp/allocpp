#ifndef ALLOC_TYPE_TRAITS_HPP
#define ALLOC_TYPE_TRAITS_HPP

#include <type_traits>
#include <memory>

#include "is_swappable.hpp"

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
{
    int member;
};

/* *****************************************************************************************************
   has_
   ***************************************************************************************************** */

template <typename T, typename = void>
struct has_rebind: public std::false_type
{};

template <typename T>
struct has_rebind<T, details::void_t<typename T::template rebind<details::placeholder>>>: public std::true_type
{};


template <typename T, typename = void>
struct has_dereference_operator: public std::false_type
{};

template <typename T>
struct has_dereference_operator<T, void_t<decltype(*std::declval<T>())>>: public std::is_convertible<
    decltype(*std::declval<T>()),
    typename std::add_lvalue_reference<typename std::pointer_traits<T>::element_type>::type
    >
{};


template <typename T, typename = void>
struct has_member_access_operator: public std::false_type
{};

template <typename T>
struct has_member_access_operator<T, void_t<decltype(std::declval<T>().operator->())>>: public std::true_type
{};

/* *****************************************************************************************************
   supports_
   ***************************************************************************************************** */

template <typename T, typename U, typename = void>
struct supports_equality: public std::false_type
{};

template <typename T, typename U>
struct supports_equality<
        T,
        U,
        details::void_t<
            decltype(std::declval<T>() == std::declval<U>()),
            decltype(std::declval<U>() == std::declval<T>())
            >
    >: public std::integral_constant<bool,
               std::is_same<bool, decltype(std::declval<T>() == std::declval<U>())>::value
            && std::is_same<bool, decltype(std::declval<U>() == std::declval<T>())>::value
            >
{};


template <typename T, typename U, typename = void>
struct supports_inequality: public std::false_type
{};

template <typename T, typename U>
struct supports_inequality<
        T,
        U,
        details::void_t<
            decltype(std::declval<T>() != std::declval<U>()),
            decltype(std::declval<U>() != std::declval<T>())
            >
    >: public std::integral_constant<bool,
               std::is_same<bool, decltype(std::declval<T>() != std::declval<U>())>::value
            && std::is_same<bool, decltype(std::declval<U>() != std::declval<T>())>::value
            >
{};

/* *****************************************************************************************************
   is_
   ***************************************************************************************************** */

template <typename T, typename U>
struct is_equality_comparable: public std::integral_constant<bool,
        supports_equality<T, U>::value && supports_inequality<T, U>::value
        >
{};


template <typename T, typename = void>
struct is_contextual_convertible_to_bool: public std::false_type
{};

template <typename T>
struct is_contextual_convertible_to_bool<T, void_t<
            decltype(std::declval<T>() ? true : false)
        >
    >: public std::true_type
{};

template <typename T, typename U>
using is_swappable = op::is_swappable<T, U>;

template <typename T, typename U>
using is_nothrow_swappable = op::is_nothrow_swappable<T, U>;

/* *****************************************************************************************************
   enable_
   ***************************************************************************************************** */

// helper struct for declaring inner template type rebind with template parameter U.
// enable_rebind<X, U>::type will be alias to X::rebind<U> if X defines rebind,
// otherwise it will be equal to just X

template <typename X, typename U, typename Enable = void>
struct enable_rebind
{
    typedef X type;
};

template <typename X, typename U>
struct enable_rebind<X, U, typename std::enable_if<has_rebind<X>::value>::type>
{
    typedef typename X::template rebind<U> type;
};


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

} // namespace details

} // namespace alloc_utility

#endif // ALLOC_TYPE_TRAITS_HPP
