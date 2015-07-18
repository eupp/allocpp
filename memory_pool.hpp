#ifndef MEMORY_POOL_HPP
#define MEMORY_POOL_HPP

#include <cstdint>

#include "alloc_traits.hpp"
#include "pointer_cast.hpp"

namespace alloc_utility
{

namespace details
{

template <typename T, typename alloc_traits = allocation_traits<T>>
class chunk
{
public:

    typedef T value_type;
    typedef typename alloc_traits::pointer pointer;
    typedef typename alloc_traits::reference reference;
    typedef typename alloc_traits::size_type size_type;
    typedef typename alloc_traits::difference_type difference_type;

    typedef alloc_traits allocation_traits;

    template <typename U>
    using rebind = chunk<U, typename alloc_traits::template rebind<U>>;

    template <typename U>
    using rebind_pointer = typename alloc_traits::template rebind_pointer<U>;

    bool is_memory_available() const
    {
        return m_available > 0;
    }

private:
    static const int CHUNK_SIZE = std::UINT8_MAX;

    pointer m_chunk;
    std::uint8_t m_head;
    std::uint8_t m_available;
};

}

}

#endif // MEMORY_POOL_HPP
