#ifndef ALLOCPP_DYNAMIC_REGION_HPP
#define ALLOCPP_DYNAMIC_REGION_HPP

#include <liballocpp/concepts/concepts.hpp>
#include <liballocpp/concepts/tags.hpp>
#include <liballocpp/ptrs/block_ptr.hpp>
#include <liballocpp/utils/pointer.hpp>

namespace allocpp { namespace plcs {

template <typename Ptr>
class dynamic_region
{
    typedef ptrs::block_ptr<Ptr> block_ptr;
public:
    typedef Ptr pointer_type;
    typedef typename utils::pointer_traits<pointer_type>::size_type size_type;
    typedef concepts::dynamic_region_tag concept_tag;

    ALLOCPP_CONCEPT_ASSERT((concepts::RandomAccessPtr<pointer_type>));

    dynamic_region(const block_ptr& block)
        : m_block(block)
    {}

    pointer_type memory() const
    {
        return m_block.ptr();
    }

    size_type size() const
    {
        return m_block.size();
    }

    pointer_type begin() const
    {
        return m_block.ptr();
    }

    pointer_type end() const
    {
        return m_block.ptr() + m_block.size();
    }

    bool owns(pointer_type p)
    {
        return (begin() <= p) && (p < end());
    }
private:
    block_ptr m_block;
};

}}

#endif //ALLOCPP_DYNAMIC_REGION_HPP
