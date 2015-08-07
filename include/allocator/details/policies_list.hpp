#ifndef POLICIES_LIST_HPP
#define POLICIES_LIST_HPP

namespace alloc_utility
{

namespace details
{

    // helper class for iterate through policies
    // it keeps information about types of policies

    template <typename alloc_traits, typename... alloc_policies>
    class policies_list
    {};

    template <typename alloc_traits, typename alloc_policy, typename... alloc_policies>
    class policies_list<alloc_traits, alloc_policy, alloc_policies...>
            : public alloc_policy::template rebind_base<policies_list<alloc_traits, alloc_policies...>>
    {
        typedef typename alloc_policy::template rebind_base<policies_list<alloc_traits, alloc_policies...>> base;

    public:

        typedef typename alloc_traits::pointer pointer;
        typedef typename alloc_traits::size_type size_type;

        template <typename U>
        using rebind = policies_list<
                                        typename alloc_traits::template rebind<U>,
                                        typename alloc_policy::template rebind<U>,
                                        typename alloc_policies::template rebind<U>...
                                    >;

        policies_list() = default;

        template <typename U>
        policies_list(const policies_list::rebind<U>& other):
            base(other)
        {}

        template <typename U>
        bool operator==(const policies_list::rebind<U>& other) const noexcept
        {
            return base::operator==(other);
        }

        template <typename U>
        bool operator!=(const policies_list::rebind<U>& other) const noexcept
        {
            return base::operator!=(other);
        }
    };

    template <typename alloc_traits>
    class policies_list<alloc_traits>: public none_policy<typename alloc_traits::value_type, alloc_traits>
    {
    public:

        template <typename U>
        using rebind = policies_list<typename alloc_traits::template rebind<U>>;

        policies_list() = default;

        template <typename U>
        policies_list(const policies_list::rebind<U>& other)
        {
            ALLOC_UNUSED(other);
        }

        template <typename U>
        bool operator==(const policies_list::rebind<U>& other) const noexcept
        {
            ALLOC_UNUSED(other);
            return true;
        }

        template <typename U>
        bool operator!=(const policies_list::rebind<U>& other) const noexcept
        {
            return !operator==(other);
        }
    };

} // namespace details

} // namespace alloc_utility

#endif // POLICIES_LIST_HPP
