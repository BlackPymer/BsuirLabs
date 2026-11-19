cmake_minimum_required(VERSION 3.24)

if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    project(problem-solver VERSION 0.1.0 LANGUAGES CXX)

    set(CMAKE_CXX_STANDARD 17)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)
    set(CMAKE_CXX_EXTENSIONS OFF)

    set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
    set(SC_EXTENSIONS_DIRECTORY ${CMAKE_BINARY_DIR}/extensions)

    option(SC_BUILD_TESTS "Build module tests" ON)
    if(SC_BUILD_TESTS)
        include(GoogleTest)
        find_package(GTest REQUIRED)
    endif()

    find_package(sc-machine REQUIRED)
endif()

if(NOT DEFINED SC_EXTENSIONS_DIRECTORY)
    set(SC_EXTENSIONS_DIRECTORY ${CMAKE_BINARY_DIR}/extensions)
endif()

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/bank-processing-module)
