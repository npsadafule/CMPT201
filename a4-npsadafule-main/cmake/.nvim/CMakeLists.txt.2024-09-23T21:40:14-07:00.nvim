cmake_minimum_required(VERSION 3.22)

project(
  CMakeExercise
  VERSION 1.0
  DESCRIPTION "This is a CMake exercise. "
  LANGUAGES C)

add_executable(random src/main.c src/random_range.c)

include_directories(include)

file(GLOB SOURCES "src/*.c")

add_executable(add ${SOURCES})
