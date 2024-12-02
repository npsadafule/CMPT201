cmake_minimum_required(VERSION 3.10)

project(BlockChain)

#Enable compile_commands.json generation
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED True)

include_directories(include)

set(SOURCES
  src/main.c
  src/blockchain.c
)

#find OpenSSL lib for SHA256 functionality
find_package(OpenSSL REQUIRED)
if(NOT OPENSSL_FOUND)
  message(FATAL_ERROR "OpenSSL lib not found")
endif()

add_executable(blockchain ${SOURCES})

#library search path and link ARM library
target_link_directories(blockchain PRIVATE lib)

#link libraries
target_link_libraries(blockchain PRIVATE ${OPENSSL_CRYPTO_LIBRARY} check-aarch64 -fsanitize=address)

#Add OpenSSL include directories
target_include_directories(blockchain PRIVATE ${OPENSSL_INCLUDE_DIR})

#Output executable to build
set_target_properties(blockchain PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
)
