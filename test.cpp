#include <iostream>
#include <cassert>
#include <vector>

#include "allocator.hpp"
#include "alloc_policies.hpp"

using namespace alloc_utility;

struct C
{
    int m_i;

    C(int i):
        m_i(i)
    {}
};

typedef allocator<int, allocation_traits<int>, default_allocation_policy<int>> simple_int_allocator;
typedef allocator<C, allocation_traits<C>, default_allocation_policy<C>> simple_object_allocator;

void test_allocate()
{
    simple_int_allocator alloc;
    const int size = 10;
    int* p = alloc.allocate(size);

    assert(p);
    for (int i = 0; i < size; ++i) {
        p[i] = i;
    }
    alloc.deallocate(p, size);
}

void test_address()
{
    simple_int_allocator alloc;
    int* p = alloc.allocate(1);

    assert(alloc.address(*p) == p);
    alloc.deallocate(p, 1);
}

void test_construct()
{
    simple_object_allocator alloc;
    C* p = alloc.allocate(1);
    alloc.construct<C, int>(p, 42);

    assert(p->m_i == 42);
    alloc.destroy(p);
    alloc.deallocate(p, 1);
}

void test_maxsize()
{
    simple_int_allocator alloc;
    assert(alloc.max_size() >= 0);
}

void test_stl_vector()
{
    std::vector<int, simple_int_allocator> vec;
    for (int i = 0; i < 1000; ++i) {
        vec.push_back(i);
    }
}

void test_logging()
{
    typedef allocator<int, allocation_traits<int>,
                      default_allocation_policy<int>,
                      logging_policy<int>
                     > logging_allocator;
    logging_allocator alloc;
    alloc.set_log(&std::cout);
    std::vector<int, logging_allocator> vec(alloc);
    for (int i = 0; i < 1000; ++i) {
        vec.push_back(i);
    }
}

int main()
{
    test_allocate();
    test_address();
    test_construct();
    test_maxsize();
    test_stl_vector();

    test_logging();

    return 0;
}

