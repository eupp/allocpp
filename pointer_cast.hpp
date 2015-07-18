#ifndef POINTER_CAST_HPP
#define POINTER_CAST_HPP

namespace alloc_utility
{

template <typename ptr_to, typename ptr_from>
struct pointer_cast_traits
{
    static ptr_to static_pointer_cast(const ptr_from& ptr)
    {
        return static_cast<ptr_to>(ptr);
    }

    static ptr_to dynamic_pointer_cast(const ptr_from& ptr)
    {
        return dynamic_cast<ptr_to>(ptr);
    }

    static ptr_to reinterpet_pointer_cast(const ptr_from& ptr)
    {
        return reinterpret_cast<ptr_to>(ptr);
    }

};

}

#endif // POINTER_CAST_HPP
