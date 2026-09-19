cmake_minimum_required(VERSION 3.16)
project(pizzeria VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(include)

set(CORE_SOURCES
    src/common/pizzeria_exceptions.cpp
    src/human/human.cpp
    src/human/intern.cpp
    src/menu/menu_item.cpp
    src/menu/pizza.cpp
    src/menu/ingredient.cpp
    src/menu/recipe.cpp
    src/menu/combo_set.cpp
    src/menu/menu_catalog.cpp
    src/order/order.cpp
    src/staff/pizzeria_worker.cpp
    src/staff/pizzeria_cooker.cpp
    src/staff/pizzeria_cooker_intern.cpp
    src/staff/pizzeria_waiter.cpp
    src/staff/cashier.cpp
    src/staff/call_center_operator.cpp
    src/staff/pizzeria_manager.cpp
    src/staff/accountant.cpp
    src/staff/hostess.cpp
    src/staff/courier.cpp
    src/delivery/delivery_address.cpp
    src/delivery/delivery_zone.cpp
    src/delivery/delivery_route.cpp
    src/delivery/delivery_tracker.cpp
    src/delivery/delivery_fee_calculator.cpp
    src/payment/payment.cpp
    src/payment/cash_payment.cpp
    src/payment/card_payment.cpp
    src/payment/mobile_payment.cpp
    src/payment/receipt.cpp
    src/payment/coupon.cpp
    src/payment/loyalty_program.cpp
    src/kitchen/kitchen.cpp
    src/kitchen/oven.cpp
    src/kitchen/refrigerator.cpp
    src/kitchen/warehouse.cpp
    src/kitchen/supplier.cpp
    src/kitchen/quality_controller.cpp
    src/pizzeria/pizzeria_department.cpp
    src/pizzeria/pizzeria_client.cpp
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