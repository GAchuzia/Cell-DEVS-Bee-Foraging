# Compiler
CXX = g++
CXXFLAGS = -std=c++17 -O3 -g

# Paths (user-based defaults; override if your setup differs)
CADMIUM_PATH ?= $(HOME)/rt_cadmium/include
JSON_PATH ?= $(HOME)/libs

INCLUDES = -I$(CADMIUM_PATH) -I$(JSON_PATH) -I./src

SRC = src/main.cpp
TARGET = nectar

# Filter Cadmium CSV in-place for Cell-DEVS Web Viewer: keep header + rows with
# cell coordinates (x,y) and 8-field state in column 5. Writes to .tmp then mv.
define FILTER_LOG_FOR_WEB_VIEWER
	awk -F';' 'NR<=2 || ($$3 ~ /^\([0-9]+,[0-9]+\)$$/ && $$5 ~ /^<[^,]+,[^,]+,[^,]+,[^,]+,[^,]+,[^,]+,[^,]+,[^,]+>$$/) { print }' "$(1)" > "$(1).tmp" && mv -f "$(1).tmp" "$(1)"
endef

.PHONY: all clean run tests test1 test2 test3 test4 test5 test6 results_dir

all: $(TARGET)
	@echo "Build successful: ./$(TARGET) is ready."
	@echo "Next steps:"
	@echo "  make run      # run default simulation"
	@echo "  make test1    # run one test scenario"
	@echo "  make tests    # run all six test scenarios"
	@echo "Note: use make targets to generate DEVS Web Viewer-ready CSV files."

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) $(SRC) $(INCLUDES) -o $(TARGET)

results_dir:
	@mkdir -p simulation_results

clean:
	rm -f $(TARGET)

# Default scenario: config/nectarVisualization_config.json -> simulation_results/grid_log.csv
run: $(TARGET) results_dir
	@echo ""
	@rm -f simulation_results/grid_log.csv
	@echo "Running default pair:"
	@echo "  config: config/nectarVisualization_config.json"
	@echo "  output: simulation_results/grid_log.csv"
	./$(TARGET)
	$(call FILTER_LOG_FOR_WEB_VIEWER,simulation_results/grid_log.csv)
	@echo ""
	@echo "Done. Load in viewer:"
	@echo "  config/nectarVisualization_config.json + simulation_results/grid_log.csv"

test1: $(TARGET) results_dir
	@echo ""
	@echo "=== Running test1 ==="
	./$(TARGET) config/tests/test1_no_bees_config.json simulation_results/test1_grid_log.csv
	$(call FILTER_LOG_FOR_WEB_VIEWER,simulation_results/test1_grid_log.csv)
	@echo "Done: viewer-ready file at simulation_results/test1_grid_log.csv"

test2: $(TARGET) results_dir
	@echo ""
	@echo "=== Running test2 ==="
	./$(TARGET) config/tests/test2_center_burst_config.json simulation_results/test2_grid_log.csv
	$(call FILTER_LOG_FOR_WEB_VIEWER,simulation_results/test2_grid_log.csv)
	@echo "Done: viewer-ready file at simulation_results/test2_grid_log.csv"

test3: $(TARGET) results_dir
	@echo ""
	@echo "=== Running test3 ==="
	./$(TARGET) config/tests/test3_corner_unwrapped_config.json simulation_results/test3_unwrapped_grid_log.csv
	$(call FILTER_LOG_FOR_WEB_VIEWER,simulation_results/test3_unwrapped_grid_log.csv)
	@echo "Done: viewer-ready file at simulation_results/test3_unwrapped_grid_log.csv"

test4: $(TARGET) results_dir
	@echo ""
	@echo "=== Running test4 ==="
	./$(TARGET) config/tests/test4_corner_wrapped_config.json simulation_results/test4_wrapped_grid_log.csv
	$(call FILTER_LOG_FOR_WEB_VIEWER,simulation_results/test4_wrapped_grid_log.csv)
	@echo "Done: viewer-ready file at simulation_results/test4_wrapped_grid_log.csv"

test5: $(TARGET) results_dir
	@echo ""
	@echo "=== Running test5 ==="
	./$(TARGET) config/tests/test5_multi_species_config.json simulation_results/test5_multi_species_grid_log.csv
	$(call FILTER_LOG_FOR_WEB_VIEWER,simulation_results/test5_multi_species_grid_log.csv)
	@echo "Done: viewer-ready file at simulation_results/test5_multi_species_grid_log.csv"

test6: $(TARGET) results_dir
	@echo ""
	@echo "=== Running test6 ==="
	./$(TARGET) config/tests/test6_river_config.json simulation_results/test6_river_grid_log.csv
	$(call FILTER_LOG_FOR_WEB_VIEWER,simulation_results/test6_river_grid_log.csv)
	@echo "Done: viewer-ready file at simulation_results/test6_river_grid_log.csv"

tests: test1 test2 test3 test4 test5 test6
	@echo ""
	@echo "All tests completed."
