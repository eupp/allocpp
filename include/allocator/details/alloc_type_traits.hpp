#ifndef ALLOC_TYPE_TRAITS_HPP
#define ALLOC_TYPE_TRAITS_HPP

#include <type_traits>
#include <memory>
#include <cstddef>

#include "pointer_cast.hpp"
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
   pre-declarations
   ***************************************************************************************************** */

template <typename T, typename = void>
struct enable_difference_type;

template <typename T, typename = void>
struct is_contextual_convertible_to_bool;

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


template <typename T, typename = void>
struct has_array_subscript_operator: public std::false_type
{};

template <typename T>
struct has_array_subscript_operator<
    T,
    void_t<decltype(std::declval<T>()[std::declval<typename enable_difference_type<T>::type>()])>
    >: public std::is_convertible<
        decltype(std::declval<T>()[std::declval<typename enable_difference_type<T>::type>()]),
        typename std::add_lvalue_reference<int>::type
        >
{};


template <typename T, typename = void>
struct has_pre_increment_operator: public std::false_type
{};

template <typename T>
struct has_pre_increment_operator<
    T,
    void_t<decltype(++std::declval<typename std::add_lvalue_reference<T>::type>())>
    >: public std::is_convertible<
        decltype(++std::declval<typename std::add_lvalue_reference<T>::type>()),
        typename std::add_lvalue_reference<T>::type
        >
{};

template <typename T, typename = void>
struct has_pre_decrement_operator: public std::false_type
{};

template <typename T>
struct has_pre_decrement_operator<
    T,
    void_t<decltype(--std::declval<typename std::add_lvalue_reference<T>::type>())>
    >: public std::is_convertible<
        decltype(--std::declval<typename std::add_lvalue_reference<T>::type>()),
        typename std::add_lvalue_reference<T>::type
        >
{};


template <typename T, typename = void>
struct has_post_increment_operator: public std::false_type
{};

template <typename T>
struct has_post_increment_operator<
    T,
    void_t<decltype(std::declval<typename std::add_lvalue_reference<T>::type>()++)>
    >: public std::is_convertible<
        decltype(std::declval<typename std::add_lvalue_reference<T>::type>()++),
        typename std::add_lvalue_reference<typename std::add_const<T>::type>::type
    >
{};


template <typename T, typename = void>
struct has_post_decrement_operator: public std::false_type
{};

template <typename T>
struct has_post_decrement_operator<
    T,
    void_t<decltype(std::declval<typename std::add_lvalue_reference<T>::type>()--)>
    >: public std::is_convertible<
        decltype(std::declval<typename std::add_lvalue_reference<T>::type>()--),
        typename std::add_lvalue_reference<typename std::add_const<T>::type>::type
    >
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
    void_t<
        decltype(std::declval<T>() == std::declval<U>()),
        decltype(std::declval<U>() == std::declval<T>())
        >
    >: public std::integral_constant<bool,
               is_contextual_convertible_to_bool<decltype(std::declval<T>() == std::declval<U>())>::value
            && is_contextual_convertible_to_bool<decltype(std::declval<U>() == std::declval<T>())>::value
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
               is_contextual_convertible_to_bool<decltype(std::declval<T>() != std::declval<U>())>::value
            && is_contextual_convertible_to_bool<decltype(std::declval<U>() != std::declval<T>())>::value
            >
{};


template <typename Res, typename lOp, typename rOp, typename = void>
struct supports_addition: public std::false_type
{};

template <typename Res, typename lOp, typename rOp>
struct supports_addition<
        Res, lOp, rOp, void_t<
            decltype(std::declval<lOp>() + std::declval<rOp>())
            >
        >: public std::is_same<
                  Res,
                  decltype(std::declval<lOp>() + std::declval<rOp>())
                  >
{};


template <typename Res, typename lOp, typename rOp, typename = void>
struct supports_substraction: public std::false_type
{};

template <typename Res, typename lOp, typename rOp>
struct supports_substraction<
        Res, lOp, rOp, void_t<
            decltype(std::declval<lOp>() - std::declval<rOp>())
            >
        >: public std::is_same<
                  Res,
                  decltype(std::declval<lOp>() - std::declval<rOp>())
                  >
{};


template <typename lT, typename rT, typename = void>
struct supports_add_assign: public std::false_type
{};

template <typename lT, typename rT>
struct supports_add_assign<
        lT, rT, void_t<
            decltype(std::declval<typename std::add_lvalue_reference<lT>::type>() += std::declval<rT>())
            >
        >: public std::is_convertible<
                decltype(std::declval<typename std::add_lvalue_reference<lT>::type>() += std::declval<rT>()),
                typename std::add_lvalue_reference<lT>::type
        >
{};


template <typename lT, typename rT, typename = void>
struct supports_sub_assign: public std::false_type
{};

template <typename lT, typename rT>
struct supports_sub_assign<
        lT, rT, void_t<
            decltype(std::declval<typename std::add_lvalue_reference<lT>::type>() -= std::declval<rT>())
            >
        >: public std::is_convertible<
                decltype(std::declval<typename std::add_lvalue_reference<lT>::type>() -= std::declval<rT>()),
                typename std::add_lvalue_reference<lT>::type
        >
{};

/* *****************************************************************************************************
   is_
   ***************************************************************************************************** */

template <typename T, typename>
struct is_contextual_convertible_to_bool: public std::false_type
{};

template <typename T>
struct is_contextual_convertible_to_bool<T, void_t<
            decltype(std::declval<T>() ? true : false)
        >
    >: public std::true_type
{};


template <typename T, typename U>
struct is_equality_comparable: public std::integral_constant<bool,
        supports_equality<T, U>::value && supports_inequality<T, U>::value
        >
{};


template <typename T, typename U, typename = void>
struct is_less_than_comparable: public std::false_type
{};

template <typename T, typename U>
struct is_less_than_comparable<T, U, void_t<
        decltype(std::declval<T>() < std::declval<U>())
        >
    >: public is_contextual_convertible_to_bool<
                decltype(std::declval<T>() < std::declval<U>())
                >
{};


template <typename T, typename U, typename = void>
struct is_greater_than_comparable: public std::false_type
{};

template <typename T, typename U>
struct is_greater_than_comparable<T, U, void_t<
        decltype(std::declval<T>() > std::declval<U>())
        >
    >: public is_contextual_convertible_to_bool<
                decltype(std::declval<T>() > std::declval<U>())
                >
{};


template <typename T, typename U, typename = void>
struct is_less_equal_comparable: public std::false_type
{};

template <typename T, typename U>
struct is_less_equal_comparable<T, U, void_t<
        decltype(std::declval<T>() <= std::declval<U>())
        >
    >: public is_contextual_convertible_to_bool<
                decltype(std::declval<T>() <= std::declval<U>())
                >
{};

template <typename T, typename U, typename = void>
struct is_greater_equal_comparable: public std::false_type
{};

template <typename T, typename U>
struct is_greater_equal_comparable<T, U, void_t<
        decltype(std::declval<T>() >= std::declval<U>())
        >
    >: public is_contextual_convertible_to_bool<
                decltype(std::declval<T>() >= std::declval<U>())
                >
{};

template <typename T, typename U>
using is_swappable = op::is_swappable<T, U>;

template <typename T, typename U>
using is_nothrow_swappable = op::is_nothrow_swappable<T, U>;


template <typename T, typename = void>
struct is_constructible_from_rebind: public std::false_type
{};

template <typename T>
struct is_constructible_from_rebind<T, typename std::enable_if<has_rebind<T>::value>::type>:
//        public std::true_type

        public std::integral_constant<bool,
        std::is_constructible<T, typename T::template rebind<details::placeholder>>::value
        >
{};


// this trait checks is object of pointer-like class (which is keeps memory occupied by objects
// of type pointer::element_type) could be created by reinterpret_pcast from pointer-like class
// of same type but with different inner (element) type U.

template <typename pointer, typename U, typename = void>
struct is_uninterpretable_memory: public std::false_type
{};


template <typename pointer, typename U>
struct is_uninterpretable_memory<pointer, U, void_t<
        decltype(
            pointer_cast_traits<pointer>::reinterpret_pcast(
                std::declval<typename pointer_cast_traits<pointer>::template rebind<U>>()
            ))
        >
    >: public std::true_type
{};

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


template <typename T, typename>
struct enable_difference_type
{
    typedef std::ptrdiff_t type;
};

template <typename T>
struct enable_difference_type<T, void_t<typename T::difference_type>>
{
    typedef typename T::difference_type type;
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
