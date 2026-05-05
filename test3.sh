#!/bin/bash
# test3.sh - Unwrapped boundaries: west species-B band plus southeast species-A hotspot on hard edges.
# Input config:  config/tests/test3_corner_unwrapped_config.json
# Output log:    simulation_results/test3_unwrapped_grid_log.csv
mkdir -p simulation_results
./bin/nectar config/tests/test3_corner_unwrapped_config.json simulation_results/test3_unwrapped_grid_log.csv
awk -F';' 'NR<=2 || ($3 ~ /^\([0-9]+,[0-9]+\)$/ && $5 ~ /^<[^,]+,[^,]+,[^,]+,[^,]+,[^,]+,[^,]+,[^,]+,[^,]+>$/) { print }' "simulation_results/test3_unwrapped_grid_log.csv" > "simulation_results/test3_unwrapped_grid_log.csv.tmp" && mv -f "simulation_results/test3_unwrapped_grid_log.csv.tmp" "simulation_results/test3_unwrapped_grid_log.csv"