CXX      = g++
CXXFLAGS = -std=c++11 -Wall -Wextra -Iinclude

TEST_TARGET = unit_tests
TEST_SRC    = src/unit_tests.cpp src/tic_tac_toe.cpp

MAIN_TARGET = tic_tac_toe
MAIN_SRC    = src/main.cpp src/tic_tac_toe.cpp

.PHONY: all tests main docs clean

all: main tests

tests: $(TEST_TARGET)

$(TEST_TARGET): $(TEST_SRC) include/tic_tac_toe.hpp
	$(CXX) $(CXXFLAGS) -o $@ $(TEST_SRC) -lUnitTest++

main: $(MAIN_TARGET)

$(MAIN_TARGET): $(MAIN_SRC) include/tic_tac_toe.hpp
	$(CXX) $(CXXFLAGS) -o $@ $(MAIN_SRC)

docs:
	doxygen Doxyfile

clean:
	rm -f $(TEST_TARGET) $(MAIN_TARGET)
	rm -rf docs