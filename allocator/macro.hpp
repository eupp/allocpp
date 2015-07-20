#ifndef MACRO_HPP
#define MACRO_HPP

#define DECLARE_ALLOC_TRAITS(T, alloc_traits)                           \
    typedef T value_type;                                               \
    typedef typename alloc_traits::pointer pointer;                     \
    typedef typename alloc_traits::reference reference;                 \
    typedef typename alloc_traits::const_pointer const_pointer;         \
    typedef typename alloc_traits::const_reference const_reference;     \
    typedef typename alloc_traits::size_type size_type;                 \
    typedef typename alloc_traits::difference_type difference_type;     \
    typedef alloc_traits allocation_traits;

#define DECLARE_REBIND_ALLOC(alloc, alloc_base, T, alloc_traits)        \
    template <typename U>                                               \
    using rebind = alloc<                                               \
                            U,                                          \
                            typename alloc_traits::template rebind<U>,  \
                            typename alloc_base::template rebind<U>     \
                        >;                                              \
    template <typename policy>                                          \
    using rebind_base = alloc<                                          \
                                T,                                      \
                                alloc_traits,                           \
                                typename policy::template rebind<T>     \
                             >;

#define DECLARE_ALLOC_POLICY_WT(alloc, alloc_base, T, alloc_traits)     \
    DECLARE_ALLOC_TRAITS(T, alloc_traits)                               \
    DECLARE_REBIND_ALLOC(alloc, alloc_base, T, alloc_traits)

#endif // MACRO_HPP
