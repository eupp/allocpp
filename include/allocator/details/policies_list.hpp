#ifndef POLICIES_LIST_HPP
#define POLICIES_LIST_HPP

#include <type_traits>

#include "none_policy.hpp"
#include "alloc_type_traits.hpp"

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
        typedef policies_list<alloc_traits, alloc_policies...> super_base;

    public:

        typedef typename alloc_traits::pointer pointer;
        typedef typename alloc_traits::size_type size_type;

        template <typename U>
        using rebind = policies_list<
                                        typename alloc_traits::template rebind<U>,
                                        typename alloc_policy::template rebind<U>,
                                        typename alloc_policies::template rebind<U>...
                                    >;

        typedef std::integral_constant<bool,
                   enable_propagate_on_copy<alloc_policy>::value
                || super_base::propagate_on_container_copy_assignment::value
            > propagate_on_container_copy_assignment;

        typedef std::integral_constant<bool,
                   enable_propagate_on_move<alloc_policy>::value
                || super_base::propagate_on_container_move_assignment::value
            > propagate_on_container_move_assignment;

        typedef std::integral_constant<bool,
                   enable_propagate_on_move<alloc_policy>::value
                || super_base::propagate_on_container_swap::value
            > propagate_on_container_swap;

        policies_list() = default;
        policies_list(const policies_list&) = default;
        policies_list(policies_list&&) = default;

        template <typename U>
        policies_list(const rebind<U>& other):
            base(other)
        {}

        bool operator==(const policies_list& other) const
        {
            return equal_to_helper<alloc_policy>(other);
        }

        bool operator!=(const policies_list& other) const
        {
            return not_equal_to_helper<alloc_policy>(other);
        }

    private:

        template <typename policy>
        auto equal_to_helper(const policies_list& other) const
            -> typename std::enable_if<details::supports_equality<policy, policy>::value, bool>::type
        {
            return base::operator==(other) && super_base::operator==(other);
        }

        template <typename policy>
        auto equal_to_helper(const policies_list& other) const
            -> typename std::enable_if<!details::supports_equality<policy, policy>::value, bool>::type
        {
            return super_base::operator==(other);
        }

        template <typename policy>
        auto not_equal_to_helper(const policies_list& other) const
            -> typename std::enable_if<details::supports_inequality<policy, policy>::value, bool>::type
        {
            return base::operator!=(other) || super_base::operator!=(other);
        }

        template <typename policy>
        auto not_equal_to_helper(const policies_list& other) const
            -> typename std::enable_if<!details::supports_inequality<policy, policy>::value, bool>::type
        {
            return super_base::operator!=(other);
        }
    };

    template <typename alloc_traits>
    class policies_list<alloc_traits>: public none_policy<typename alloc_traits::value_type, alloc_traits>
    {
    public:

        template <typename U>
        using rebind = policies_list<typename alloc_traits::template rebind<U>>;

        policies_list() = default;
        policies_list(const policies_list&) = default;
        policies_list(const policies_list&&) = default;

        template <typename U>
        policies_list(const policies_list::rebind<U>& other)
        {
            ALLOC_UNUSED(other);
        }

        policies_list& operator=(const policies_list&) = default;
        policies_list& operator=(policies_list&&) = default;

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
