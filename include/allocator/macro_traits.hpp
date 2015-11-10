#ifndef MACRO_TRAITS_HPP
#define MACRO_TRAITS_HPP

#include <type_traits>

#include "details/rebind.hpp"

#define DECLARE_VOID_ALLOC_TRAITS(void_alloc_traits)                            \
    typedef typename void_alloc_traits::void_pointer void_pointer;              \
    typedef typename void_alloc_traits::const_void_pointer const_void_pointer;  \
    typedef typename void_alloc_traits::size_type size_type;                    \
    typedef typename void_alloc_traits::difference_type difference_type;        \
    typedef void_alloc_traits void_allocation_traits_t;


#define DECLARE_ALLOC_TRAITS(alloc_traits)                                      \
    typedef typename alloc_traits::value_type value_type;                       \
    typedef typename alloc_traits::pointer pointer;                             \
    typedef typename alloc_traits::const_pointer const_pointer;                 \
    typedef typename alloc_traits::void_pointer void_pointer;                   \
    typedef typename alloc_traits::const_void_pointer const_void_pointer;       \
    typedef typename alloc_traits::reference reference;                         \
    typedef typename alloc_traits::const_reference const_reference;             \
    typedef typename alloc_traits::size_type size_type;                         \
    typedef typename alloc_traits::difference_type difference_type;             \
    typedef alloc_traits allocation_traits_t;                                   \
                                                                                \
    template <typename U>                                                       \
    using rebind_pointer = typename alloc_traits::template rebind_pointer<U>;   \
    template <typename U>                                                       \
    using rebind_const_pointer = typename alloc_traits::template rebind_const_pointer<U>;


//#define DECLARE_REBIND_ALLOC(alloc, base, T...)                                  \
//    template <typename U>                                                       \
//    using rebind = ::alloc_utility::details::rebind<alloc, U, __VA_ARGS__>;     \

#define DECLARE_REBIND_ALLOC(alloc, ...)                                        \
    template <typename traits, typename policy>                                 \
    using rebind_alloc = alloc<__VA_ARGS__, traits, policy>;

#define CHECK_IS_REBINDED_ALLOC(alloc)                                           \
    static_assert(std::is_same<alloc, rebind<typename alloc::value_type>>::value,\
        "Type should be rebinded allocation policy");

#define DECLARE_ARBITRATOR(alloc, arbitrator)                                   \
    private:                                                                    \
        arbitrator m_alloc_arbitrator;                                          \
    public:                                                                     \
        arbitrator* get_arbitrator(alloc*)                                      \
        {                                                                       \
            return &m_alloc_arbitrator;                                         \
        }

#define ALLOC_UNUSED(expr) while(0) { (void)(expr); }

#endif // MACRO_TRAITS_HPP
