set(allocpp_BENCHMARKS
        main.cpp
        malloc.cpp
        )

include_directories(${CMAKE_SOURCE_DIR}/allocpp/include)

find_package(Threads REQUIRED)
find_package(Boost 1.54 COMPONENTS system chrono)

add_definitions(-DNONIUS_USE_BOOST_CHRONO)

add_executable(allocpp_benchmark ${allocpp_BENCHMARKS})
target_link_libraries(allocpp_benchmark ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries(allocpp_benchmark ${Boost_LIBRARIES})
target_link_libraries(allocpp_benchmark liballocpp)