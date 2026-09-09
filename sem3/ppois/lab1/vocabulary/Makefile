CXX      = g++
CXXFLAGS = -std=c++11 -Wall -Wextra -Iinclude

TEST_TARGET = unit_tests
TEST_SRC    = src/unit_tests.cpp src/vocabulary.cpp

MAIN_TARGET = vocabulary
MAIN_SRC    = src/main.cpp src/vocabulary.cpp

.PHONY: all tests main docs clean

all: main tests

tests: $(TEST_TARGET)

$(TEST_TARGET): $(TEST_SRC) include/vocabulary.hpp
	$(CXX) $(CXXFLAGS) -o $@ $(TEST_SRC) -lUnitTest++

main: $(MAIN_TARGET)

$(MAIN_TARGET): $(MAIN_SRC) include/vocabulary.hpp
	$(CXX) $(CXXFLAGS) -o $@ $(MAIN_SRC)

docs:
	doxygen Doxyfile

clean:
	rm -f $(TEST_TARGET) $(MAIN_TARGET)
	rm -rf docs