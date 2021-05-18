cmake_minimum_required(VERSION 3.14)
add_executable(unittests
    main.cpp
)

set_property(TARGET unittests PROPERTY CXX_STANDARD 14)
set_property(TARGET unittests PROPERTY CXX_STANDARD_REQUIRED TRUE)
set_property(TARGET unittests PROPERTY CXX_EXTENSIONS OFF)

target_compile_options(unittests
    PRIVATE -Wall -Wextra -Wpedantic --coverage
)
target_link_options(unittests
    PRIVATE --coverage
)

# GoogleTest
include(FetchContent)
FetchContent_Declare(googletest
  URL https://github.com/google/googletest/archive/master.zip
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

target_link_libraries(unittests
    PRIVATE cpplibsocket
    PRIVATE gmock
    PRIVATE gtest
)

add_custom_target(coverage
    COMMAND ${CMAKE_SOURCE_DIR}/test/coverage.sh ${CMAKE_SOURCE_DIR}/test
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)

add_dependencies(coverage unittests)
add_test(NAME unit-tests COMMAND unittests)
