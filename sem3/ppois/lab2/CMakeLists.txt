cmake_minimum_required(VERSION 3.16)
project(pizzeria VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(include)

add_executable(pizzeria
    src/main.cpp
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