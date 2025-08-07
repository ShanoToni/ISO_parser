cmake_minimum_required(VERSION 3.10)
project(ISOParser)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(ISOParser main.cpp)

set(INPUT_FILE "transcript.txt" CACHE STRING "Path to input transcript file")

target_compile_definitions(ISOParser PRIVATE INPUT_FILE_PATH="${INPUT_FILE}")
