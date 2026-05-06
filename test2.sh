#!/bin/bash
# test2.sh - Center foragers: four center bees and two butterflies on an unwrapped grid.
# Input config:  config/tests/test2_center_burst_config.json
# Output log:    simulation_results/test2_grid_log.csv
mkdir -p simulation_results
./bin/nectar config/tests/test2_center_burst_config.json simulation_results/test2_grid_log.csv
awk -F';' 'NR<=2 || ($3 ~ /^\([0-9]+,[0-9]+\)$/ && $5 ~ /^<[^,]+,[^,]+,[^,]+,[^,]+,[^,]+,[^,]+,[^,]+,[^,]+>$/) { print }' "simulation_results/test2_grid_log.csv" > "simulation_results/test2_grid_log.csv.tmp" && mv -f "simulation_results/test2_grid_log.csv.tmp" "simulation_results/test2_grid_log.csv"