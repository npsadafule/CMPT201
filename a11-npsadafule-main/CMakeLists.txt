# CMakeLists.txt
cmake_minimum_required(VERSION 3.10)
project(ChatProject C)

set(CMAKE_C_STANDARD 11)

# Include directories
include_directories(include)

# Server executable
add_executable(server src/server.c src/helper.c)

# Client executable
add_executable(client src/client.c src/helper.c)

# Link libraries if necessary (e.g., pthread)
find_package(Threads REQUIRED)
target_link_libraries(server PRIVATE Threads::Threads)
target_link_libraries(client PRIVATE Threads::Threads)