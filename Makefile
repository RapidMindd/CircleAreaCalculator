CXX = g++
CXXFLAGS = -std=c++20 -Wall -Wextra -O2
SRC = main.cpp
OUT = a.out
ARGS ?= 2.0 1'000'000 10

.PHONY: run bench clean

run: $(OUT)
	./$(OUT) $(ARGS)

$(OUT): $(SRC)
	$(CXX) $(CXXFLAGS) $(SRC) -o $(OUT)

clean:
	@rm -f $(OUT)
