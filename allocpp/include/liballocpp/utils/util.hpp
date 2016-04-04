#ifndef ALLOCPP_UTIL_HPP
#define ALLOCPP_UTIL_HPP

namespace allocpp { namespace utils {

struct noncopyable
{
    noncopyable() = default;

    noncopyable(const noncopyable&) = delete;
    noncopyable& operator=(const noncopyable&) = delete;

    noncopyable(noncopyable&&) = default;
    noncopyable& operator=(noncopyable&&) = default;
};

struct nonmovable
{
    nonmovable() = default;

    nonmovable(const nonmovable&) = default;
    nonmovable& operator=(const nonmovable&) = default;

    nonmovable(nonmovable&&) = delete;
    nonmovable& operator=(nonmovable&&) = delete;
};

struct nonassignable
{
    nonassignable& operator=(const nonassignable&) = delete;
    nonassignable& operator=(nonassignable&&) = delete;
};

}}

#endif //ALLOCPP_UTIL_HPP
