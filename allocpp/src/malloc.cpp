#include <liballocpp/plcs/malloc.hpp>

#include <cassert>
#include <cstddef>

namespace allocpp { namespace plcs {

alloc_response<byte*> malloc::allocate(const alloc_request<byte*>& req)
{
    using namespace ptrs;
    if (req.alignment() <= alignof(std::max_align_t)) {
        byte* ptr = reinterpret_cast<byte*>(std::malloc(req.size()));
        return alloc_response<byte*>::builder()
                .set_memory_block(block_ptr<byte*>(ptr, req.size(), req.alignment()))
                .build();
    }
    return alloc_response<byte*>();
}

dealloc_response<byte*> malloc::deallocate(const dealloc_request<byte*>& req)
{
    std::free(req.ptr());
    return dealloc_response<byte*>();
}

ownership malloc::owns(byte*)
{
    return ownership::UNDEFINED;
}

}}

