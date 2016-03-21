FIND_PACKAGE(Boost 1.54)

set(allocpp_LIB_HDRS
        include/liballocpp/concept/concept.hpp
        )

message(WARNING ${Boost_INCLUDE_DIR})

include_directories(include/liballocpp)
include_directories(${Boost_INCLUDE_DIR})

add_library(liballocpp INTERFACE)
target_include_directories(liballocpp INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/liballocpp>
        $<INSTALL_INTERFACE:include/liballocpp>
        )

install(FILES ${allocpp_LIB_HDRS} DESTINATION ${CMAKE_INSTALL_PREFIX}/include/liballocpp)