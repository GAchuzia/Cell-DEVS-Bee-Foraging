# Compiler
CXX = g++
CXXFLAGS = -std=c++17 -O3 -g

# Paths (override on the command line if needed):
#   make CADMIUM_PATH=/path/to/cadmium/include JSON_PATH=/path/to/json/parent
#
# Defaults are intentionally user-agnostic. If dependencies are not on a system include
# path, set CADMIUM_PATH and JSON_PATH when invoking make.
CADMIUM_PATH ?=
JSON_PATH ?=

INCLUDES = -I./src

ifneq ($(strip $(CADMIUM_PATH)),)
INCLUDES += -I$(CADMIUM_PATH)
endif

ifneq ($(strip $(JSON_PATH)),)
INCLUDES += -I$(JSON_PATH)
endif

SRC = src/main.cpp
TARGET = nectar

.PHONY: all clean run tests test1 test2 test3 test4 test5 results_dir

all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) $(SRC) $(INCLUDES) -o $(TARGET)

results_dir:
	@mkdir -p simulation_results

clean:
	rm -f $(TARGET)

# Default scenario: config/nectarVisualization_config.json -> simulation_results/grid_log.csv
run: all results_dir
	./$(TARGET)

test1: all results_dir
	./$(TARGET) config/tests/test1_no_bees_config.json simulation_results/test1_grid_log.csv

test2: all results_dir
	./$(TARGET) config/tests/test2_center_burst_config.json simulation_results/test2_grid_log.csv

test3: all results_dir
	./$(TARGET) config/tests/test3_corner_unwrapped_config.json simulation_results/test3_unwrapped_grid_log.csv

test4: all results_dir
	./$(TARGET) config/tests/test4_corner_wrapped_config.json simulation_results/test4_wrapped_grid_log.csv

test5: all results_dir
	./$(TARGET) config/tests/test5_multi_species_config.json simulation_results/test5_multi_species_grid_log.csv

tests: test1 test2 test3 test4 test5