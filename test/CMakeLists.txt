enable_testing()

set(allocpp_TESTS
        pointer_decorator_test.cpp)

find_package(Threads REQUIRED)

find_library(GMOCK_LIB NAMES libgmock.a
        PATHS
        /usr/lib
        /usr/lib64
        /usr/local/lib
        /usr/local/lib64
        )

find_library(GMOCK_MAIN_LIB NAMES libgmock_main.a
        PATHS
        /usr/lib
        /usr/lib64
        /usr/local/lib
        /usr/local/lib64
        )

message(WARNING $<TARGET_PROPERTY:liballocpp, INTERFACE_INCLUDE_DIRECTORIES>)

include_directories(${CMAKE_SOURCE_DIR}/allocpp/include)

add_executable(allocpp_test ${allocpp_TESTS})
target_link_libraries(allocpp_test ${GMOCK_LIB})
target_link_libraries(allocpp_test ${GMOCK_MAIN_LIB})
target_link_libraries(allocpp_test ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries(allocpp_test LINK_INTERFACE_LIBRARIES liballocpp)

add_test(all_tests allocpp_test)