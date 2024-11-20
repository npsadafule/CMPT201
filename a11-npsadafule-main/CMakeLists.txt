# CMakeLists.txt
cmake_minimum_required(VERSION 3.10)
project(GroupChatServer)

set(CMAKE_C_STANDARD 11)

# Include directories
include_directories(include)

# Server executable
add_executable(server src/server.c src/helper.c)
target_link_libraries(server pthread)

# Client executable
add_executable(client src/client.c src/helper.c)
target_link_libraries(client pthread)