cmake_minimum_required(VERSION 3.10)
project(tokenizer CXX)

set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_STANDARD 17)

set(SOURCES ${CMAKE_SOURCE_DIR}/src/main.cpp)
add_executable(tokenizer ${SOURCES})

target_include_directories(tokenizer PRIVATE ${CMAKE_SOURCE_DIR}/inc)

set(CMAKE_CXX_FLAGS "-Wall -Werror -Wconversion -fno-rtti -fno-exceptions")
