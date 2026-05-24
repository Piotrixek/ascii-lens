cmake_minimum_required(VERSION 3.29.3)
project(AsciiLens LANGUAGES CXX)

set(CMAKE_CXX_EXTENSIONS OFF)

include(FetchContent)

FetchContent_Declare(
    stb
    GIT_REPOSITORY https://github.com/nothings/stb.git
    GIT_TAG        master
)

FetchContent_GetProperties(stb)
if(NOT stb_POPULATED)
    FetchContent_Populate(stb)
    
    add_library(stb_image INTERFACE)
    target_include_directories(stb_image INTERFACE ${stb_SOURCE_DIR})
endif()

add_executable(ascii_art main.cpp)

target_link_libraries(ascii_art PRIVATE stb_image)

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    target_compile_options(ascii_art PRIVATE -std=c++26 -Wall -Wextra -Wpedantic -O3)
    target_link_libraries(ascii_art PRIVATE stdc++exp)
endif()
