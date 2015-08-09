#ifndef TEST_TYPES_HPP
#define TEST_TYPES_HPP

struct class_with_rebind
{
    template <typename T>
    using rebind = T;
};

struct class_without_rebind
{};

#endif // TEST_TYPES_HPP
