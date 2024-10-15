cmake_minimum_required(VERSION 3.14)

#Project name
project(SimpleShell C CXX)

# GoogleTest requires at least C++14
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#Include the directory with header files
include_directories(include)

include(FetchContent)
FetchContent_Declare(
  googletest
  DOWNLOAD_EXTRACT_TIMESTAMP false
  URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip)
FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(shell_test gtest/shell_test.cpp)
target_link_libraries(shell_test GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(shell_test)

add_executable(shell src/shell.c)
target_link_libraries(shell PRIVATE pthread)

if (CMAKE_C_COMPILER_ID STREQUAL "GNU" OR CMAKE_C_COMPILER_ID STREQUAL "Clang")
  target_compile_options(shell PRIVATE -fsanitize=address -fno-omit-frame-pointer)
  target_link_libraries(shell PRIVATE -fsanitize=address)
endif()
