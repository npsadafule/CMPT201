cmake_minimum_required(VERSION 3.10)

#Project Name
project(memory_allocator)

#Set the C standard
set(CMAKE_C_STANDARD 99)

#Add include directory
include_directories(include)

#Add source files
add_executable(main src/main.c src/alloc.c)

#Enable debugging and sanitizers
set(CMAKE_C_FLAGS_DEBUG "-g -fsanitize=address,undefined")
set(CMAKE_C_FLAGS_RELEASE "-O2")

#Set the target for the executable
target_link_libraries(main)
