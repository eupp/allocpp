TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt
CONFIG += c++11

SOURCES += \
    test/smoke_test.cpp \
    test/policies_test.cpp \
    test/pool_test.cpp \
    test/statistic_policy_test.cpp

HEADERS += \
    allocator.hpp \
    alloc_policies.hpp \
    alloc_traits.hpp \
    pointer_cast.hpp \
    pool_allocation.hpp \
    allocator/allocator.hpp \
    allocator/alloc_policies.hpp \
    allocator/alloc_traits.hpp \
    allocator/pointer_cast.hpp \
    allocator/pool_allocation.hpp \
    allocator/macro.hpp \
    allocator/statistic_policy.hpp

INCLUDEPATH += allocator

LIBS += -lgtest -lgtest_main -pthread

