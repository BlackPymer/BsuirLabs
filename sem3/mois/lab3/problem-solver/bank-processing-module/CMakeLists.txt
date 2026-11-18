file(GLOB SOURCES CONFIGURE_DEPENDS
    "*.cpp" "*.hpp"
    "agent/*.cpp" "agent/*.hpp"
    "keynodes/*.hpp"
)

add_library(bank-processing-module SHARED ${SOURCES})

target_link_libraries(bank-processing-module
    LINK_PUBLIC sc-machine::sc-memory
)

target_include_directories(bank-processing-module
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}
)

set_target_properties(bank-processing-module
    PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${SC_EXTENSIONS_DIRECTORY}
)

if(SC_BUILD_TESTS)
    file(GLOB TEST_SOURCES CONFIGURE_DEPENDS
        "test/*.cpp"
    )

    add_executable(bank-processing-module-tests ${TEST_SOURCES})

    target_link_libraries(bank-processing-module-tests
        LINK_PRIVATE GTest::gtest_main
        LINK_PRIVATE bank-processing-module
    )

    target_include_directories(bank-processing-module-tests
        PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}
    )

    gtest_discover_tests(bank-processing-module-tests
        TEST_LIST ${TEST_SOURCES}
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/test
    )
endif()
