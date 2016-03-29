#ifndef ALLOCPP_UTIL_HPP
#define ALLOCPP_UTIL_HPP

namespace allocpp { namespace utils {

class noncopyable
{
public:
    noncopyable() = default;

    noncopyable(const noncopyable&) = delete;
    noncopyable& operator=(const noncopyable&) = delete;

    noncopyable(noncopyable&&) = default;
    noncopyable& operator=(noncopyable&&) = default;
};

class nonmovable
{
public:
    nonmovable() = default;

    nonmovable(const nonmovable&) = default;
    nonmovable& operator=(const nonmovable&) = default;

    nonmovable(nonmovable&&) = delete;
    nonmovable& operator=(nonmovable&&) = delete;
};


}}

#endif //ALLOCPP_UTIL_HPP
