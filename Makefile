# Compiler
CXX = g++
CXXFLAGS = -Iinclude -Wall -std=c++17

# Source files
SRC = src/main.cpp src/dashboard.cpp src/uptime.cpp src/logger.cpp

# Output
OUT = uptimer

all: $(OUT)

$(OUT): $(SRC)
	$(CXX) $(CXXFLAGS) $(SRC) -o $(OUT)

run: $(OUT)
	./$(OUT)

clean:
	rm -f $(OUT)
