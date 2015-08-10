#ifndef REBIND_BASE_HPP
#define REBIND_BASE_HPP

#include <type_traits>

#include "alloc_type_traits.hpp"

namespace alloc_utility
{

namespace details
{

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

// helper struct for replacng last type from variadic template list with rebind_type

template <int N, template <typename...> class alloc, typename T, typename rebind_type, typename head_type, typename... types>
struct rebind_base_helper
{
    typedef typename rebind_base_helper<N-1, alloc, T, rebind_type, types..., head_type>::type type;
};

template <template <typename...> class alloc, typename T, typename rebind_type, typename head_type, typename... types>
struct rebind_base_helper<0, alloc, T, rebind_type, head_type, types...>
{
    typedef alloc<T, types..., typename rebind_type::template rebind<T>> type;
};

template <template <typename...> class alloc, typename T, typename rebind_type, typename head_type>
struct rebind_base_helper<0, alloc, T, rebind_type, head_type>
{
    typedef alloc<T, typename rebind_type::template rebind<T>> type;
};

// helper alias for declaring rebind types
// consider rebind<alloc, U, T1, T2> and suppose T1 has inner template rebind and T2 not,
// then rebind<alloc, U, T1, T2>::type will be equal to alloc<U, T1::rebind<U>, T2>

template <template <typename...> class alloc, typename U, typename... types>
using rebind = alloc<U, typename enable_rebind<types, U>::type...>;

// helper alias for declaring rebind_base

template <template <typename...> class alloc, typename T, typename policy, typename... types>
using rebind_base = typename rebind_base_helper<sizeof...(types)-1, alloc, T, policy, types...>::type;

} // namespace details

}

#endif // REBIND_BASE_HPP
