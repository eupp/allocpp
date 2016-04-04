FIND_PACKAGE(Boost 1.54)

set(allocpp_LIB_HDRS
        include/liballocpp/alloc_messaging.hpp
        include/liballocpp/types.hpp

        include/liballocpp/concepts/concepts.hpp
        include/liballocpp/concepts/tags.hpp

        include/liballocpp/plcs/null.hpp
        include/liballocpp/plcs/malloc.hpp

        include/liballocpp/ptrs/pointer_decorator.hpp
        include/liballocpp/ptrs/block_ptr.hpp

        include/liballocpp/utils/any.hpp
        include/liballocpp/utils/pointer.hpp
        include/liballocpp/utils/util.hpp
        )

set(allocpp_LIB_SRCS
        src/malloc.cpp)

include_directories(include/)
include_directories(${Boost_INCLUDE_DIR})

add_library(liballocpp ${allocpp_LIB_HDRS} ${allocpp_LIB_SRCS})
target_include_directories(liballocpp INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/liballocpp>
        $<INSTALL_INTERFACE:include/liballocpp>
        )

install(FILES ${allocpp_LIB_HDRS} DESTINATION ${CMAKE_INSTALL_PREFIX}/include/liballocpp)