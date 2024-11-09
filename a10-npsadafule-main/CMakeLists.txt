# Minimum CMake version required
cmake_minimum_required(VERSION 3.10)

# Project name and language
project(MapReduce C)

# Set the C standard to C11
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED True)

# Include directories
include_directories(include)

# Add the source files
set(SOURCE_FILES
    src/free_output.c
    src/main.c
    src/map_and_reduce.c
    src/number_of_mappers_reducers.c
    src/partition.c
    src/single_map.c
    src/single_reduce.c
    src/test.c
    src/interface.c
)

# Add the executable
add_executable(mapreduce ${SOURCE_FILES})

# Enable warnings and treat warnings as errors
if (CMAKE_C_COMPILER_ID STREQUAL "GNU" OR CMAKE_C_COMPILER_ID STREQUAL "Clang")
    # Suppress unused parameter warnings for the entire target
    target_compile_options(mapreduce PRIVATE -Wall -Wextra -Werror -Wno-unused-parameter)
endif()

# Link libraries if necessary (e.g., pthread)
target_link_libraries(mapreduce pthread)
