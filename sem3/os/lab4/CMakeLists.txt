cmake_minimum_required(VERSION 3.16)
project(os_lab4 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(filesystem STATIC src/filesystem.cpp)
target_include_directories(filesystem PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(filesystem PRIVATE -Wall -Wextra)
endif()

file(SIZE ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp MAIN_SIZE)
if(MAIN_SIZE GREATER 0)
    add_executable(main src/main.cpp)
    target_link_libraries(main PRIVATE filesystem)
endif()
