#include <nonius/nonius.h++>

#include <cstdlib>
#include <vector>

#include <liballocpp/plcs/malloc.hpp>

using namespace allocpp;

static const size_t MALLOC_SIZE = 64;

NONIUS_BENCHMARK("malloc", [](nonius::chronometer meter)
{
    std::vector<void*> ptrs(meter.runs());
    meter.measure([&ptrs] (size_t i) {
        ptrs[i] = std::malloc(MALLOC_SIZE);
        size_t* s = reinterpret_cast<size_t*>(ptrs[i]);
        *s = 42;
        return;
    });
    for (auto& ptr: ptrs) {
        std::free(ptr);
    }
});

NONIUS_BENCHMARK("malloc_policy", [](nonius::chronometer meter)
{
    std::vector<alloc_response<byte*>> rsps(meter.runs());
    plcs::malloc malloc_plc;
    auto alloc_rsp = alloc_request<byte*>::builder().set_size(MALLOC_SIZE).build();
    meter.measure([&rsps, &malloc_plc, &alloc_rsp] (size_t i) {
        rsps[i] = malloc_plc.allocate(alloc_rsp);
        size_t* s = reinterpret_cast<size_t*>(rsps[i].ptr());
        *s = 42;
        return;
    });
    for (auto& rsp: rsps) {
        malloc_plc.deallocate(dealloc_request<byte*>::builder()
                                      .set_memory_block(rsp.memory_block())
                                      .build());
    }
});