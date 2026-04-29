# Compiler
CXX = g++
CXXFLAGS = -std=c++17 -O3 -g

# Paths (edit manually per machine)
CADMIUM_PATH ?= /home/cadmium/rt_cadmium/include
JSON_PATH ?= /home/Achuzia/libs

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
	@echo "  make run      # run default visualization pair"
	@echo "  make test1    # run one test scenario"
	@echo "  make tests    # run all six test scenarios"

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) $(SRC) $(INCLUDES) -o $(TARGET)

results_dir:
	@mkdir -p simulation_results

clean:
	rm -f $(TARGET)

# Default scenario: config/nectarVisualization_config.json -> simulation_results/grid_log.csv
run: all results_dir
	@rm -f simulation_results/grid_log.csv
	@echo "Running default pair:"
	@echo "  config: config/nectarVisualization_config.json"
	@echo "  output: simulation_results/grid_log.csv"
	./$(TARGET)
	$(call FILTER_LOG_FOR_WEB_VIEWER,simulation_results/grid_log.csv)
	@echo "Done. Load in viewer:"
	@echo "  config/nectarVisualization_config.json + simulation_results/grid_log.csv"

test1: all results_dir
	./$(TARGET) config/tests/test1_no_bees_config.json simulation_results/test1_grid_log.csv
	$(call FILTER_LOG_FOR_WEB_VIEWER,simulation_results/test1_grid_log.csv)

test2: all results_dir
	./$(TARGET) config/tests/test2_center_burst_config.json simulation_results/test2_grid_log.csv
	$(call FILTER_LOG_FOR_WEB_VIEWER,simulation_results/test2_grid_log.csv)

test3: all results_dir
	./$(TARGET) config/tests/test3_corner_unwrapped_config.json simulation_results/test3_unwrapped_grid_log.csv
	$(call FILTER_LOG_FOR_WEB_VIEWER,simulation_results/test3_unwrapped_grid_log.csv)

test4: all results_dir
	./$(TARGET) config/tests/test4_corner_wrapped_config.json simulation_results/test4_wrapped_grid_log.csv
	$(call FILTER_LOG_FOR_WEB_VIEWER,simulation_results/test4_wrapped_grid_log.csv)

test5: all results_dir
	./$(TARGET) config/tests/test5_multi_species_config.json simulation_results/test5_multi_species_grid_log.csv
	$(call FILTER_LOG_FOR_WEB_VIEWER,simulation_results/test5_multi_species_grid_log.csv)

test6: all results_dir
	./$(TARGET) config/tests/test6_river_config.json simulation_results/test6_river_grid_log.csv
	$(call FILTER_LOG_FOR_WEB_VIEWER,simulation_results/test6_river_grid_log.csv)

tests: test1 test2 test3 test4 test5 test6
