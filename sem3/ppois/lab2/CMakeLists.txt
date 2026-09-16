cmake_minimum_required(VERSION 3.16)
project(pizzeria VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(include)

set(CORE_SOURCES
    src/human.cpp
    src/pizza.cpp
    src/order.cpp
    src/pizzeria_worker.cpp
    src/pizzeria_cooker.cpp
    src/pizzeria_cooker_intern.cpp
    src/pizzeria_waiter.cpp
    src/pizzeria_client.cpp
    src/pizzeria_department.cpp
    src/intern.cpp
)

add_executable(pizzeria
    src/main.cpp
    ${CORE_SOURCES}
)

find_library(UNITTEST_LIB UnitTest++ REQUIRED)
find_path(UNITTEST_INCLUDE UnitTest++.h PATH_SUFFIXES UnitTest++ REQUIRED)

add_executable(run_tests
    tests/test_main.cpp
    tests/tests.cpp
    ${CORE_SOURCES}
)
target_include_directories(run_tests PRIVATE ${UNITTEST_INCLUDE})
target_link_libraries(run_tests PRIVATE ${UNITTEST_LIB})

target_compile_options(run_tests PRIVATE --coverage)
target_link_options(run_tests PRIVATE --coverage)
