#ifndef ALLOCPP_ALLOC_MESSAGING_HPP
#define ALLOCPP_ALLOC_MESSAGING_HPP

#include <bitset>

#include <liballocpp/ptrs/block_ptr.hpp>
#include <liballocpp/utils/pointer.hpp>
#include <liballocpp/utils/any.hpp>
#include <liballocpp/utils/util.hpp>

namespace allocpp {

namespace details {

class alloc_flags : public std::bitset<sizeof(size_t)>
{
public:
    static const size_t FLAGS_COUNT = sizeof(size_t);
};

}

template <typename pointer_type>
class alloc_request
{
public:
    typedef typename utils::pointer_traits<pointer_type>::size_type size_type;

    static const size_t FLAGS_COUNT = details::alloc_flags::FLAGS_COUNT;

    alloc_request()
        : m_hint(nullptr)
        , m_size(0)
        , m_alignment(0)
    {}

    alloc_request(const alloc_request&) = default;
    alloc_request(alloc_request&&) = default;

    alloc_request& operator=(const alloc_request&) = default;
    alloc_request& operator=(alloc_request&&) = default;

    size_type size() const
    {
        return m_size;
    }

    size_type alignment() const
    {
        return m_alignment;
    }

    bool flag(size_t i) const
    {
        return m_flags[i];
    }

    pointer_type hint() const
    {
        return m_hint;
    }

    bool extra_empty() const
    {
        return m_extra.empty();
    }

    utils::any extra() const
    {
        return m_extra;
    }

    class builder : private utils::noncopyable, private utils::nonmovable
    {
    public:
        builder() = default;

        builder& set_size(size_type size)
        {
            m_request.m_size = size;
            return *this;
        }

        builder& set_alignment(size_type alignment)
        {
            m_request.m_alignment = alignment;
            return *this;
        }

        builder& set_flag(size_t i, bool flag)
        {
            m_request.m_flags[i] = flag;
            return *this;
        }

        builder& set_hint(pointer_type hint)
        {
            m_request.m_hint = hint;
            return *this;
        }

        builder& set_extra(const utils::any& extra)
        {
            m_request.m_extra = extra;
            return *this;
        }

        alloc_request build() const
        {
            return m_request;
        }
    private:
        alloc_request m_request;
    };
private:
    pointer_type m_hint;
    size_type m_size;
    size_type m_alignment;
    details::alloc_flags m_flags;
    utils::any m_extra;
};

template <typename pointer_type>
class alloc_response
{
public:
    typedef typename utils::pointer_traits<pointer_type>::size_type size_type;
    typedef ptrs::block_ptr<pointer_type> block_ptr;

    static const size_t FLAGS_COUNT = details::alloc_flags::FLAGS_COUNT;

    alloc_response()
        : m_block(nullptr)
        , m_alignment(0)
    {}

    alloc_response(const alloc_response&) = default;
    alloc_response(alloc_response&&) = default;

    alloc_response& operator=(const alloc_response&) = default;
    alloc_response& operator=(alloc_response&&) = default;

    block_ptr memory_block() const
    {
        return m_block;
    }

    pointer_type ptr() const
    {
        return m_block.ptr();
    }

    size_type size() const
    {
        return m_block.size();
    }

    size_type alignment() const
    {
        return m_alignment;
    }

    bool flag(size_t i) const
    {
        return m_flags[i];
    }

    bool extra_empty() const
    {
        return m_extra.empty();
    }

    utils::any extra() const
    {
        return m_extra;
    }

    class builder : private utils::noncopyable, private utils::nonmovable
    {
    public:
        builder() = default;

        builder& set_memory_block(const block_ptr& block)
        {
            m_response.m_block = block;
            return *this;
        }

        builder& set_alignment(size_type alignment)
        {
            m_response.m_alignment = alignment;
            return *this;
        }

        builder& set_flag(size_t i, bool flag)
        {
            m_response.m_flags[i] = flag;
            return *this;
        }

        builder& set_extra(const utils::any& extra)
        {
            m_response.m_extra = extra;
            return *this;
        }

        alloc_response build() const
        {
            return m_response;
        }
    private:
        alloc_response m_response;
    };
private:
    block_ptr m_block;
    size_type m_alignment;
    details::alloc_flags m_flags;
    utils::any m_extra;
};

template <typename pointer_type>
class dealloc_request
{
public:
    typedef typename utils::pointer_traits<pointer_type>::size_type size_type;
    typedef ptrs::block_ptr<pointer_type> block_ptr;

    static const size_t FLAGS_COUNT = details::alloc_flags::FLAGS_COUNT;

    dealloc_request()
        : m_block(nullptr)
    {}

    dealloc_request(const dealloc_request&) = default;
    dealloc_request(dealloc_request&&) = default;

    dealloc_request& operator=(const dealloc_request&) = default;
    dealloc_request& operator=(dealloc_request&&) = default;

    block_ptr memory_block() const
    {
        return m_block;
    }

    pointer_type ptr() const
    {
        return m_block.ptr();
    }

    size_type size() const
    {
        return m_block.size();
    }

    bool size_set() const
    {
        return size() != 0;
    }

    bool flag(size_t i) const
    {
        return m_flags[i];
    }

    bool extra_empty() const
    {
        return m_extra.empty();
    }

    utils::any extra() const
    {
        return m_extra;
    }

    class builder : private utils::noncopyable, private utils::nonmovable
    {
    public:
        builder() = default;

        builder& set_memory_block(const block_ptr& block)
        {
            m_request.m_block = block;
            return *this;
        }

        builder& set_ptr(pointer_type ptr)
        {
            m_request.m_block = block_ptr(ptr, 0);
            return *this;
        }

        builder& set_flag(size_t i, bool flag)
        {
            m_request.m_flags[i] = flag;
            return *this;
        }

        builder& set_extra(const utils::any& extra)
        {
            m_request.m_extra = extra;
            return *this;
        }

        dealloc_request build() const
        {
            return m_request;
        }
    private:
        dealloc_request m_request;
    };
private:
    block_ptr m_block;
    details::alloc_flags m_flags;
    utils::any m_extra;
};

template <typename pointer_type>
class dealloc_response
{
public:
    typedef typename utils::pointer_traits<pointer_type>::size_type size_type;

    static const size_t FLAGS_COUNT = details::alloc_flags::FLAGS_COUNT;

    dealloc_response() = default;

    dealloc_response(const dealloc_response&) = default;
    dealloc_response(dealloc_response&&) = default;

    dealloc_response& operator=(const dealloc_response&) = default;
    dealloc_response& operator=(dealloc_response&&) = default;

    bool flag(size_t i) const
    {
        return m_flags[i];
    }

    bool extra_empty() const
    {
        return m_extra.empty();
    }

    utils::any extra() const
    {
        return m_extra;
    }

    class builder : private utils::noncopyable, private utils::nonmovable
    {
    public:
        builder() = default;

        builder& set_flag(size_t i, bool flag)
        {
            m_response.m_flags[i] = flag;
            return *this;
        }

        builder& set_extra(const utils::any& extra)
        {
            m_response.m_extra = extra;
            return *this;
        }

        dealloc_response build() const
        {
            return m_response;
        }
    private:
        dealloc_response m_response;
    };
private:
    details::alloc_flags m_flags;
    utils::any m_extra;
};

}

#endif //ALLOCPP_ALLOC_MESSAGING_HPP
