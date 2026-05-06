#!/bin/bash
# test6.sh - River barrier: a full river row splits the grid and constrains pollinator movement.
# Input config:  config/tests/test6_river_config.json
# Output log:    simulation_results/test6_river_grid_log.csv
mkdir -p simulation_results
./bin/nectar config/tests/test6_river_config.json simulation_results/test6_river_grid_log.csv
awk -F';' 'NR<=2 || ($3 ~ /^\([0-9]+,[0-9]+\)$/ && $5 ~ /^<[^,]+,[^,]+,[^,]+,[^,]+,[^,]+,[^,]+,[^,]+,[^,]+>$/) { print }' "simulation_results/test6_river_grid_log.csv" > "simulation_results/test6_river_grid_log.csv.tmp" && mv -f "simulation_results/test6_river_grid_log.csv.tmp" "simulation_results/test6_river_grid_log.csv"