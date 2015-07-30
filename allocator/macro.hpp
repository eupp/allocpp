#ifndef MACRO_HPP
#define MACRO_HPP

namespace alloc_utility
{

namespace details
{

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

// helper struct for declaring rebind & rebind_base types

template <template <typename...> class alloc, typename T, typename... types>
struct rebind_helper
{
    template <typename U>
    using rebind = alloc<
                            U,
                            typename types::template rebind<U>...
                        >;

    template <typename policy>
    using rebind_base = typename rebind_base_helper<sizeof...(types)-1, alloc, T, policy, types...>::type;
};

// helper trait for check that rebind type is defined in class

template <typename C>
class is_rebind_defined
{
    template <
              typename X,
              typename Y = typename X::type
             >
    struct check_helper {};

    typedef char true_type;
    struct false_type { true_type m[2]; };

    template <typename X>
    static true_type check(check_helper<X>* p);

    template <typename X>
    static false_type check(...);

public:

    static bool const value = sizeof((check<C>)(0)) == sizeof(true_type);
};

} // namespace details

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
    using rebind =                                                              \
        typename alloc_utility::details                                         \
        :: template rebind_helper<alloc, T, __VA_ARGS__>:: template rebind<U>;  \
    template <typename policy>                                                  \
    using rebind_base =                                                         \
        typename alloc_utility::details                                         \
        ::template rebind_helper<alloc, T, __VA_ARGS__>:: template rebind_base<policy>;

#define ALLOC_UNUSED(expr) while(0) { (void)(expr); }

#endif // MACRO_HPP
