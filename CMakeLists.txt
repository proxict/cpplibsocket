cmake_minimum_required(VERSION 3.0)
project(cpplibsocket VERSION 1.0.0 LANGUAGES C CXX)
set(platform ${CMAKE_SYSTEM_NAME})

include("cmake/${platform}-compiler-config.txt")
include(cmake/doxygen.txt)

set(BUILD_TESTS_STORE ${BUILD_TESTS})
set(BUILD_TESTS OFF CACHE BOOL INTERNAL FORCE)
add_subdirectory(external/lib-expected)
add_subdirectory(external/lib-optional)
set(BUILD_TESTS ${BUILD_TESTS_STORE} CACHE BOOL INTERNAL FORCE)

add_library(cpplibsocket STATIC
    src/${platform}Socket.cpp
    src/SocketBase.cpp
    src/SocketTcp.cpp
    src/SocketUdp.cpp
    src/utils.cpp
)

target_include_directories(cpplibsocket
    PUBLIC include
)

target_link_libraries(cpplibsocket
    PUBLIC lib-expected
    PUBLIC lib-optional
)
if (${platform} STREQUAL "Windows")
	target_link_libraries(cpplibsocket
		PRIVATE ws2_32.lib
		PRIVATE IPHLPAPI.lib
	)
endif()

