
cmake_minimum_required(VERSION 3.0.2)
project(allocator)

include(FindGTest)

set(CMAKE_CXX_FLAGS "-g -Wall")
add_definitions(-std=c++11)

# Options. Turn on with 'cmake -Dmyvarname=ON'.
option(test "Build all tests" ON)

include_directories(include/allocator)

if (test)
    enable_test()
    add_subdirectory(test)
endif()
