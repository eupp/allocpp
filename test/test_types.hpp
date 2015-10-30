#ifndef TEST_TYPES_HPP
#define TEST_TYPES_HPP

#include <type_traits>

#include "alloc_traits.hpp"
#include "none_policy.hpp"
#include "macro.hpp"

struct empty_class
{};

struct class_with_rebind
{
    template <typename T>
    using rebind = T;
};

struct class_with_comparison_ops
{
    bool operator==(const class_with_comparison_ops&) const
    {
        return true;
    }

    bool operator!=(const class_with_comparison_ops&) const
    {
        return false;
    }

    bool operator<(const class_with_comparison_ops&) const
    {
        return false;
    }

    bool operator<=(const class_with_comparison_ops&) const
    {
        return true;
    }
};


struct class_with_arithmetic
{
public:

    class_with_arithmetic& operator+=(const empty_class&)
    {
        return *this;
    }

    class_with_arithmetic& operator-=(const empty_class&)
    {
        return *this;
    }
};

inline empty_class operator+(const class_with_arithmetic&, const class_with_arithmetic&)
{
    return empty_class();
}

inline empty_class operator-(const class_with_arithmetic&, const class_with_arithmetic&)
{
    return empty_class();
}

template <typename T, typename alloc_traits = alloc_utility::allocation_traits<T*>,
          typename base_policy = alloc_utility::none_policy<T>>
struct well_defined_policy: public base_policy
{
    DECLARE_ALLOC_TRAITS(alloc_traits)
    DECLARE_REBIND_ALLOC(well_defined_policy, T, alloc_traits, base_policy)

    typedef std::true_type propagate_on_container_copy_assignment;
    typedef std::true_type propagate_on_container_move_assignment;
    typedef std::true_type propagate_on_container_swap;

    well_defined_policy() = default;
    well_defined_policy(const well_defined_policy&) = default;

    template <typename U>
    well_defined_policy(const rebind<U>& other):
        flag(other.flag)
    {}

    well_defined_policy& operator=(const well_defined_policy&) = default;
    well_defined_policy& operator=(well_defined_policy&&) = default;

    bool operator==(const well_defined_policy& other) const noexcept
    {
        return flag == other.flag;
    }

    bool operator!=(const well_defined_policy& other) const noexcept
    {
        return flag != other.flag;
    }

    bool flag;
};

template <typename T, typename alloc_traits, typename base_policy>
inline void swap(well_defined_policy<T, alloc_traits, base_policy>&,
                 well_defined_policy<T, alloc_traits, base_policy>&) noexcept
{}

template <typename T, typename alloc_traits = alloc_utility::allocation_traits<T*>,
          typename base_policy = alloc_utility::none_policy<T>>
struct poorly_defined_policy: public base_policy
{
    DECLARE_ALLOC_TRAITS(alloc_traits)
    DECLARE_REBIND_ALLOC(poorly_defined_policy, T, alloc_traits, base_policy)

    poorly_defined_policy() = default;
    poorly_defined_policy(const poorly_defined_policy&) = default;
    poorly_defined_policy(poorly_defined_policy&&) = default;

    template <typename U>
    poorly_defined_policy(const rebind<U>& )
    {}

    poorly_defined_policy& operator=(const poorly_defined_policy&) = default;
    poorly_defined_policy& operator=(poorly_defined_policy&&) = default;
};

template <typename T, typename alloc_traits, typename base_policy>
inline void swap(poorly_defined_policy<T, alloc_traits, base_policy>&,
                 poorly_defined_policy<T, alloc_traits, base_policy>&) noexcept
{}

namespace adl_check {

struct swappable
{
    swappable(const swappable&) = delete;
    swappable(swappable&&) = delete;
};

inline void swap(swappable&, swappable&)
{
    return;
}

struct not_swappable
{
    not_swappable(const not_swappable&) = delete;
    not_swappable(not_swappable&&) = delete;
};

}

#endif // TEST_TYPES_HPP
