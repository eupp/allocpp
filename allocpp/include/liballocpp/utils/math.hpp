#ifndef ALLOCPP_MATH_HPP
#define ALLOCPP_MATH_HPP

#include <cstddef>

namespace allocpp { namespace utils {

inline size_t msb(unsigned long long n)
{
    static const unsigned long long mask[] = {
            0x000000007FFFFFFF,
            0x000000000000FFFF,
            0x00000000000000FF,
            0x000000000000000F,
            0x0000000000000003,
            0x0000000000000001
    };

    if (n == 0) {
        return 0;
    }

    size_t hi = 64;
    size_t lo = 0;

    for (int i = 0; i < sizeof mask / sizeof mask[0]; i++) {
        size_t mi = lo + (hi - lo) / 2;

        if ((n >> mi) != 0) {
            lo = mi;
        }
        else if ((n & (mask[i] << lo)) != 0) {
            hi = mi;
        }
    }

    return lo + 1;
}

}}

#endif //ALLOCPP_MATH_HPP
