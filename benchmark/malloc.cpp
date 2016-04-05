#include <nonius/nonius.h++>

#include <cstdlib>
#include <vector>

#include <liballocpp/plcs/malloc.hpp>

using namespace allocpp;

static const size_t MALLOC_SIZE = 4096;

NONIUS_BENCHMARK("malloc", [](nonius::chronometer meter)
{
    std::vector<void*> ptrs(meter.runs());
    meter.measure([&ptrs] (size_t i) {
        void* ptr = std::malloc(MALLOC_SIZE);
        ptrs[i] = ptr;
        return ptr;
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
        auto rsp = malloc_plc.allocate(alloc_rsp);
        rsps[i] = rsp;
        return rsp;
    });
    for (auto& rsp: rsps) {
        malloc_plc.deallocate(dealloc_request<byte*>::builder()
                                      .set_memory_block(rsp.memory_block())
                                      .build());
    }
});