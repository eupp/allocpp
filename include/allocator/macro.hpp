#ifndef MACRO_HPP
#define MACRO_HPP

#include <type_traits>

#include "details/rebind.hpp"

namespace alloc_utility
{

} // namespace alloc_utility

#define DECLARE_ALLOC_TRAITS(T, alloc_traits)                                   \
    typedef T value_type;                                                       \
    typedef typename alloc_traits::pointer pointer;                             \
    typedef typename alloc_traits::const_pointer const_pointer;                 \
    typedef typename alloc_traits::void_pointer void_pointer;                   \
    typedef typename alloc_traits::const_void_pointer const_void_pointer;       \
    typedef typename alloc_traits::reference reference;                         \
    typedef typename alloc_traits::const_reference const_reference;             \
    typedef typename alloc_traits::size_type size_type;                         \
    typedef typename alloc_traits::difference_type difference_type;             \
    typedef alloc_traits allocation_traits;                                     \
                                                                                \
    template <typename U>                                                       \
    using rebind_pointer = typename alloc_traits::template rebind_pointer<U>;   \
    template <typename U>                                                       \
    using rebind_const_pointer = typename alloc_traits::template rebind_const_pointer<U>;

    #define DECLARE_REBIND_ALLOC(alloc, T, ...)                                 \
    template <typename U>                                                       \
    using rebind = ::alloc_utility::details::rebind<alloc, U, __VA_ARGS__>;     \
                                                                                \
    template <typename policy>                                                  \
    using rebind_base = ::alloc_utility::details::rebind_base<alloc, T, policy, __VA_ARGS__>;                                                         \

#define CHECK_IS_REBINDED_ALLOC(alloc)                                           \
    static_assert(std::is_same<alloc, rebind<typename alloc::value_type>>::value,\
        "Type should be rebinded allocation policy");

#define ALLOC_UNUSED(expr) while(0) { (void)(expr); }

#endif // MACRO_HPP
