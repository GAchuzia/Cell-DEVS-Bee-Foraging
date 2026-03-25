# Compiler
CXX = g++
CXXFLAGS = -std=c++17 -O3 -g

# Paths (override per user/machine: make CADMIUM_PATH=... JSON_PATH=...
# or export CADMIUM_PATH / JSON_PATH before make)
CADMIUM_PATH ?= /home/cadmium/rt_cadmium/include
JSON_PATH ?= $(HOME)/libs

# Includes
INCLUDES = -I$(CADMIUM_PATH) -I$(JSON_PATH) -I./src

# Source files
SRC = src/main.cpp
TARGET = nectar

all:
	$(CXX) $(CXXFLAGS) $(SRC) $(INCLUDES) -o $(TARGET)

clean:
	rm -f $(TARGET)
