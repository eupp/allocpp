// source code is taken from https://github.com/orlp/libop/blob/master/bits/type_traits.h

/*
 * Copyright (c) 2015 Orson Peters <orsonpeters@gmail.com>
 *
 * This software is provided 'as-is', without any express or implied warranty. In no event will the
 * authors be held liable for any damages arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose, including commercial
 * applications, and to alter it and redistribute it freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not claim that you wrote the
 *  original software. If you use this software in a product, an acknowledgment in the product
 *  documentation would be appreciated but is not required.
 *
 * 2. Altered source versions must be plainly marked as such, and must not be misrepresented as
 *  being the original software.

 * 3. This notice may not be removed or altered from any source distribution.
 */

#ifndef OP_TYPE_TRAITS_H
#define OP_TYPE_TRAITS_H

#include <cstddef>
#include <type_traits>


namespace op {
    // Inherits from std::true_type if T and U are Swappable, std::false_type if otherwise.
    template<class T, class U = T>
    struct is_swappable;

    // Same as is_swappable, with the added condition that the swap must be noexcept.
    template<class T, class U = T>
    struct is_nothrow_swappable;
}



// Implementation.
namespace op {
    namespace detail {
        namespace swap_adl_tests {
            // If swap ADL finds this then it would call std::swap otherwise (same signature).
            struct tag {};

            template<class T> tag swap(T&, T&);
            template<class T, std::size_t N> tag swap(T (&a)[N], T (&b)[N]);

            // Helper functions to test if an unqualified swap is possible, and if it becomes std::swap.
            template<class, class> std::false_type can_swap(...) noexcept(false);
            template<class T, class U, class = decltype(swap(std::declval<T&>(), std::declval<U&>()))>
            std::true_type can_swap(int) noexcept(
                noexcept(swap(std::declval<T&>(), std::declval<U&>()))
            );

            template<class, class> std::false_type uses_std(...);
            template<class T, class U>
            std::is_same<decltype(swap(std::declval<T&>(), std::declval<U&>())), tag> uses_std(int);

            // Helper templates to determine if swapping is noexcept. The C++11/14 standards have a
            // broken noexcept specification for multidimensional arrays, so we use a template solution.
            template<class T>
            struct is_std_swap_noexcept : std::integral_constant<bool,
                std::is_nothrow_move_constructible<T>::value &&
                std::is_nothrow_move_assignable<T>::value
            > { };

            template<class T, std::size_t N>
            struct is_std_swap_noexcept<T[N]> : is_std_swap_noexcept<T> { };

            template<class T, class U>
            struct is_adl_swap_noexcept : std::integral_constant<bool, noexcept(can_swap<T, U>(0))> { };
        }
    }

    template<class T, class U>
    struct is_swappable : std::integral_constant<bool,
        decltype(detail::swap_adl_tests::can_swap<T, U>(0))::value &&
            (!decltype(detail::swap_adl_tests::uses_std<T, U>(0))::value ||
                (std::is_move_assignable<T>::value && std::is_move_constructible<T>::value))
    > {};

    template<class T, std::size_t N>
    struct is_swappable<T[N], T[N]> : std::integral_constant<bool,
        decltype(detail::swap_adl_tests::can_swap<T[N], T[N]>(0))::value &&
            (!decltype(detail::swap_adl_tests::uses_std<T[N], T[N]>(0))::value ||
                is_swappable<T, T>::value)
    > {};

    template<class T, class U>
    struct is_nothrow_swappable : std::integral_constant<bool,
        is_swappable<T, U>::value && (
            (decltype(detail::swap_adl_tests::uses_std<T, U>(0))::value &&
                detail::swap_adl_tests::is_std_swap_noexcept<T>::value)
            ||
            (!decltype(detail::swap_adl_tests::uses_std<T, U>(0))::value &&
                detail::swap_adl_tests::is_adl_swap_noexcept<T, U>::value)
        )
    > {};
}

#endif
