cmake_minimum_required(VERSION 3.10)

project(MapReduce C)

set(CMAKE_C_STANDARD 11)
set(CMKAE_C_STANDARD_REQUIRED True)

include_directories(include)

set(SOURCE_FILES
  src/free_output.c
  src/map_and_reduce.c
  src/number_of_mappers_reducers.c
  src/partition.c
  src/single_map.c
  src/single_reduce.c
  src/test.c
  src/main.c
  src/interface.c
)

add_executable(mapreduce ${SOURCE_FILES})

if (CMAKE_C_COMPILER_ID STREQUAL "GNU" OR CMAKE_C_COMPILER_ID STREQUAL "Clang")
  target_compile_options(mapreduce PRIVATE -Wall -Wextra -Werror -Wno-unused-parameter) #suppressing unused parameter warning for the entire target
endif()

target_link_libraries(mapreduce pthread)
