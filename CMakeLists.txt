cmake_minimum_required(VERSION 3.14)
project(cpplibsocket VERSION 1.0.0 LANGUAGES C CXX)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

include(cmake/doxygen.txt)

set(BUILD_TESTS_STORE ${BUILD_TESTS})
set(BUILD_TESTS OFF CACHE BOOL INTERNAL FORCE)
add_subdirectory(external/lib-expected)
add_subdirectory(external/lib-optional)
set(BUILD_TESTS ${BUILD_TESTS_STORE} CACHE BOOL INTERNAL FORCE)

add_library(cpplibsocket STATIC
    src/${CMAKE_SYSTEM_NAME}Socket.cpp
    src/SocketBase.cpp
    src/SocketTcp.cpp
    src/SocketUdp.cpp
    src/utils.cpp
)

set_property(TARGET cpplibsocket PROPERTY CXX_STANDARD 14)
set_property(TARGET cpplibsocket PROPERTY CXX_STANDARD_REQUIRED TRUE)
set_property(TARGET cpplibsocket PROPERTY CXX_EXTENSIONS OFF)
set_property(TARGET cpplibsocket PROPERTY POSITION_INDEPENDENT_CODE ON)
set_property(TARGET cpplibsocket PROPERTY CXX_VISIBILITY_PRESET hidden)

if(MSVC)
    target_compile_options(cpplibsocket PRIVATE
        /W4
        /we4289 # nonstandard extension used: 'variable': loop control variable declared in the for-loop is used outside
                # the for-loop scope
        /w14555 # expression has no effect; expected expression with side- effect
        /w14905 # wide string literal cast to 'LPSTR'
        /w14906 # string literal cast to 'LPWSTR'
        /w14928 # illegal copy-initialization; more than one user-defined conversion has been implicitly applied
        /permissive- # standards conformance mode for MSVC compiler.
    )
else()
    target_compile_options(cpplibsocket PRIVATE
        -Wall
        -Wextra
        -pedantic
        -Wshadow
        -Wnon-virtual-dtor
        -Wold-style-cast
        -Woverloaded-virtual
        -Wnull-dereference
        -Wformat=2
    )
endif()

target_include_directories(cpplibsocket
    PUBLIC include
)

target_link_libraries(cpplibsocket
    PUBLIC lib-expected
    PUBLIC lib-optional
)
if (${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
	target_link_libraries(cpplibsocket
		PRIVATE ws2_32.lib
		PRIVATE IPHLPAPI.lib
	)
endif()

if (BUILD_TESTS)
    add_subdirectory(test)
endif()

