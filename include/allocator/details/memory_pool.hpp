#ifndef MEMORY_POOL_HPP
#define MEMORY_POOL_HPP

#include <cstdint>
#include <cassert>
#include <limits>
#include <vector>
#include <list>
#include <memory>
#include <new>
#include <type_traits>
#include <utility>

#include "pointer_cast.hpp"
#include "macro_traits.hpp"

namespace alloc_utility
{

namespace details
{

template <typename pointer, typename size_type>
class chunk
{
public:

    static_assert(std::is_same<typename std::pointer_traits<pointer>::element_type, std::uint8_t>::value,
                  "Type of pointed value should be uint8_t");

    static const int CHUNK_MAXSIZE = std::numeric_limits<std::uint8_t>::max();

    chunk(pointer ptr, size_type obj_size, std::uint8_t chunk_size = CHUNK_MAXSIZE) noexcept:
        m_chunk(ptr)
      , m_head(0)
      , m_available(chunk_size)
      , m_size(chunk_size)
    {
        assert(ptr);
        assert(chunk_size > 0);
        for (int i = 1; i <= chunk_size; ++i, ptr += obj_size) {
            ptr[0] = i;
        }
        // last block is initialized with chunk_size
        // it is really doesn't matter because when we reach last block m_available == 0
    }

    chunk(chunk&& other) noexcept:
        m_chunk(other.m_chunk)
      , m_head(other.m_head)
      , m_available(other.m_available)
      , m_size(other.m_size)
    {}

    chunk(const chunk&) = delete;
    chunk& operator=(const chunk&) = delete;
    chunk& operator=(chunk&&) = delete;

    bool is_memory_available() const noexcept
    {
        return m_available > 0;
    }

    bool is_owned(const pointer& ptr, size_type obj_size) const noexcept
    {
        return (m_chunk <= ptr) && (ptr < m_chunk + size() * obj_size);
    }

    std::uint8_t size() const noexcept
    {
        return m_size;
    }

    pointer allocate(size_type obj_size)
    {
        if (is_memory_available()) {
            pointer ptr = m_chunk + (m_head * obj_size);
            m_head = ptr[0];
            m_available--;
            return ptr;
        }
        return pointer(nullptr);
    }

    void deallocate(const pointer& ptr, size_type obj_size)
    {
        ptr[0] = m_head;
        m_head = (ptr - m_chunk) / obj_size;
        m_available++;
    }

private:
    pointer m_chunk;
    std::uint8_t m_head;
    std::uint8_t m_available;
    std::uint8_t m_size;
};

template <typename pointer, typename size_type>
class memory_block
{
public:

    typedef chunk<pointer, size_type> chunk_type;
    typedef typename std::vector<chunk_type>::iterator chunk_it;

    memory_block(pointer mem, size_type obj_size, size_type obj_num) noexcept:
        m_mem(mem)
      , m_size(obj_num)
    {
        assert(mem);
        assert(obj_num > 0);
        size_type chunks_num = (obj_num + chunk_type::CHUNK_MAXSIZE - 1) / chunk_type::CHUNK_MAXSIZE;
        size_type last_chunk_size = obj_num % chunk_type::CHUNK_MAXSIZE;
        if (last_chunk_size == 0) {
            last_chunk_size = chunk_type::CHUNK_MAXSIZE;
        }

        m_chunks.reserve(chunks_num);
        for (size_type i = 0; i < chunks_num - 1; ++i, mem += obj_size * chunk_type::CHUNK_MAXSIZE) {
            m_chunks.emplace_back(mem, obj_size, (size_type)chunk_type::CHUNK_MAXSIZE);
        }
        m_chunks.emplace_back(mem, obj_size, last_chunk_size);
    }

    bool is_memory_available() const noexcept
    {
        for (auto& chunk: m_chunks) {
            if (chunk.is_memory_available()) {
                return true;
            }
        }
        return false;
    }

    bool is_owned(const pointer& ptr, size_type obj_size) const noexcept
    {
        return (m_mem <= ptr) && (ptr < m_mem + obj_size * size());
    }

    pointer get_memory_ptr() const noexcept
    {
        return m_mem;
    }

    size_type size() const noexcept
    {
        return m_size;
    }

    std::pair<pointer, chunk_it> allocate(size_type obj_size)
    {
        for (auto it = m_chunks.begin(); it != m_chunks.end(); ++it) {
            if (it->is_memory_available()) {
                return make_pair(it->allocate(obj_size), it);
            }
        }
        return make_pair(nullptr, m_chunks.end());
    }

    void deallocate(const pointer& ptr, size_type obj_size)
    {
        for (auto& chunk: m_chunks) {
            if (chunk.is_owned(ptr, obj_size)) {
                chunk.deallocate(ptr, obj_size);
            }
        }
    }

private:
    pointer m_mem;
    size_type m_size;
    std::vector<chunk_type> m_chunks;
};

template <typename pointer, typename size_type>
class memory_pool
{
public:

    typedef memory_block<pointer, size_type> memory_block_type;

    class memory_blocks_range
    {
    public:
        typedef typename std::vector<memory_block_type>::const_iterator memory_block_it;

        memory_blocks_range(const memory_block_it& first, const memory_block_it& last) noexcept:
            m_begin(first)
          , m_end(last)
        {}

        memory_block_it begin() const noexcept
        {
            return m_begin;
        }

        memory_block_it end() const noexcept
        {
            return m_end;
        }

        bool is_empty() const noexcept
        {
            return m_begin == m_end;
        }

        size_type count() const noexcept
        {
            return m_end - m_begin;
        }

    private:
        memory_block_it m_begin;
        memory_block_it m_end;
    };

    explicit memory_pool(size_type obj_size) noexcept:
        m_obj_size(obj_size)
    {}

    memory_pool(memory_pool&& other) noexcept:
        m_blocks(std::move(other.m_blocks))
      , m_obj_size(other.m_obj_size)
    {}

    size_type obj_size() const noexcept
    {
        return m_obj_size;
    }

    size_type capacity() const noexcept
    {
        size_type cap = 0;
        for (auto& block: m_blocks) {
            cap += block.size();
        }
        return cap;
    }

    bool is_memory_available() const noexcept
    {
        if (m_last_used_chunk.is_valid() && m_last_used_chunk.get_chunk()->is_memory_available()) {
            return true;
        }
        for (auto& block: m_blocks) {
            if (block.is_memory_available()) {
                return true;
            }
        }
        return false;
    }

    bool is_owned(const pointer& ptr) const noexcept
    {
        for (auto& block: m_blocks) {
            if (block.is_owned(ptr, m_obj_size)) {
                return true;
            }
        }
        return false;
    }

    void add_mem_block(const pointer& mem, size_type size)
    {
        m_blocks.emplace_back(mem, m_obj_size, size);
    }

    memory_blocks_range get_mem_blocks() const noexcept
    {
        return memory_blocks_range(m_blocks.begin(), m_blocks.end());
    }

    pointer allocate()
    {
        if (m_last_used_chunk.is_valid() && m_last_used_chunk.get_chunk()->is_memory_available()) {
            return m_last_used_chunk.get_chunk()->allocate(m_obj_size);
        }
        for (auto& block: m_blocks) {
            if (block.is_memory_available()) {
                auto res = block.allocate(m_obj_size);
                m_last_used_chunk.set_chunk(res.second);
                return res.first;
            }
        }
        return pointer(nullptr);
    }

    void deallocate(const pointer& ptr)
    {
        for (auto& block: m_blocks) {
            if (block.is_owned(ptr, m_obj_size)) {
                block.deallocate(ptr, m_obj_size);
            }
        }
    }

private:

    class cached_chunk
    {
    public:
        typedef typename memory_block_type::chunk_it chunk_it;

        cached_chunk() noexcept:
            m_valid(false)
        {}

        void set_chunk(const chunk_it& chk) noexcept
        {
            m_valid = true;
            m_chunk = chk;
        }

        chunk_it get_chunk() const noexcept
        {
            return m_chunk;
        }

        bool is_valid() const noexcept
        {
            return m_valid;
        }

        void invalidate() noexcept
        {
            m_valid = false;
        }

    private:
        bool m_valid;
        chunk_it m_chunk;
    };

    std::vector<memory_block_type> m_blocks;
    cached_chunk m_last_used_chunk;
    size_type m_obj_size;
};

template <typename pointer, typename size_type>
class pools_manager
{
public:

    typedef memory_pool<pointer, size_type> pool_type;
    typedef std::pair<pool_type, int> pool_with_ref_count;

    class iterator
    {
        typedef typename std::list<pool_with_ref_count>::iterator inner_iterator_type;

    public:

        iterator(const inner_iterator_type& it) noexcept:
            m_inner_it(it)
        {}

        pool_type& operator*() const noexcept
        {
            return m_inner_it->first;
        }

        pool_type* operator->() const noexcept
        {
            return &m_inner_it->first;
        }

        const iterator& operator++() noexcept
        {
            ++m_inner_it;
            return *this;
        }

        iterator operator++(int) noexcept
        {
            iterator it = *this;
            ++m_inner_it;
            return it;
        }

        bool operator==(const iterator& other) const noexcept
        {
            return m_inner_it == other.m_inner_it;
        }

        bool operator!=(const iterator& other) const noexcept
        {
            return !operator==(other);
        }

    private:
        inner_iterator_type m_inner_it;
    };

    friend bool operator==(const iterator& it1, const iterator& it2) noexcept
    {
        return it1.operator==(it2);
    }

    friend bool operator!=(const iterator& it1, const iterator& it2) noexcept
    {
        return it1.operator!=(it2);
    }

    pool_type* get_pool(size_type obj_size)
    {
        pool_type* pool = get_pool(obj_size, std::nothrow);
        if (pool != nullptr) {
            return pool;
        }
        pool_type new_pool(obj_size);
        m_pools.emplace_back(std::move(new_pool), 1);
        return &m_pools.back().first;
    }

    pool_type* get_pool(size_type obj_size, std::nothrow_t) noexcept
    {
        for (auto& pool_with_rc: m_pools) {
            if (pool_with_rc.first.obj_size() == obj_size) {
                ++pool_with_rc.second;
                return &pool_with_rc.first;
            }
        }
        return nullptr;
    }

    void release_pool(size_type obj_size) noexcept
    {
        for (auto& pool_with_rc: m_pools) {
            if (pool_with_rc.first.obj_size() == obj_size) {
                --pool_with_rc.second;
            }
        }
    }

    void erase_pool(size_type obj_size) noexcept
    {
        for (auto it = m_pools.begin(); it != m_pools.end(); ++it) {
            if (it->first.obj_size() == obj_size) {
                m_pools.erase(it);
                return;
            }
        }
    }

    int get_pool_ref_count(size_type obj_size) const noexcept
    {
        for (auto& pool_with_rc: m_pools) {
            if (pool_with_rc.first.obj_size() == obj_size) {
                return pool_with_rc.second;
            }
        }
        return 0;
    }

    iterator begin() noexcept
    {
        return iterator(m_pools.begin());
    }

    iterator end() noexcept
    {
        return iterator(m_pools.end());
    }

private:
    std::list<pool_with_ref_count> m_pools;
};

}   // namespace details

} // namespace alloc_utility

#endif // MEMORY_POOL_HPP
