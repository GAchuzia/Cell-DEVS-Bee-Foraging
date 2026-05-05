#!/bin/bash
# test2.sh - Center foragers: four center bees and two butterflies on an unwrapped grid.
# Input config:  config/tests/test2_center_burst_config.json
# Output log:    simulation_results/test2_grid_log.csv
mkdir -p simulation_results
./bin/nectar config/tests/test2_center_burst_config.json simulation_results/test2_grid_log.csv
