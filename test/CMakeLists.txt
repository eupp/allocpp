
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})

file(GLOB TEST_FILES "*.cpp")

add_executable(utests ${TEST_FILES})

target_link_libraries(foo ${GTEST_BOTH_LIBRARIES})
