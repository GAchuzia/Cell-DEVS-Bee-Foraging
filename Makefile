# Compiler
CXX = g++
CXXFLAGS = -std=c++17 -O3 -g

# Paths (override per user/machine: make CADMIUM_PATH=... JSON_PATH=...
# or export CADMIUM_PATH / JSON_PATH before make)
CADMIUM_PATH ?= /home/cadmium/rt_cadmium/include
JSON_PATH ?= $(HOME)/libs

# Includes
# nlohmann/json.hpp is commonly stored under either:
#   $(JSON_PATH)/nlohmann/json.hpp
# or:
#   $(JSON_PATH)/include/nlohmann/json.hpp
# so we add both potential include roots to make this more portable across users.
INCLUDES = -I$(CADMIUM_PATH) -I$(JSON_PATH) -I$(JSON_PATH)/include -I./src

# Source files
SRC = src/main.cpp
TARGET = nectar

all:
	$(CXX) $(CXXFLAGS) $(SRC) $(INCLUDES) -o $(TARGET)

clean:
	rm -f $(TARGET)
