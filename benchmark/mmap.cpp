#include <nonius/nonius.h++>

#include <sys/mman.h>
#include <vector>

#include <liballocpp/plcs/malloc.hpp>

using namespace allocpp;

static const size_t MMAP_SIZE = 4096;

NONIUS_BENCHMARK("mmap", [](nonius::chronometer meter)
{
    std::vector<void*> ptrs(meter.runs());
    meter.measure([&ptrs] (size_t i) {
        void* ptr = mmap(0, MMAP_SIZE, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
        ptrs[i] = ptr;
        return ptr;
    });
    for (auto& ptr: ptrs) {
        munmap(ptr, MMAP_SIZE);
    }
});

