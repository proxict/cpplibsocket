cmake_minimum_required(VERSION 3.0)
project(cpplibsocket VERSION 1.0.0 LANGUAGES C CXX)
set(platform ${CMAKE_SYSTEM_NAME})

if (${platform} STREQUAL "Linux")
	include("cmake/${platform}-compiler-config.txt")
elseif (${platform} STREQUAL "Windows")
    include("cmake/${platform}-compiler-config.txt")
endif()

include(cmake/doxygen.txt)

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

if (${platform} STREQUAL "Windows")
	target_link_libraries(cpplibsocket
		PRIVATE ws2_32.lib
	)
endif()

