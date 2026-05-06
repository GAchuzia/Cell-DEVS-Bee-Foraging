#!/bin/bash
# test1.sh - No pollinators: two plant species and pollen types, zero bees and butterflies.
# Input config:  config/tests/test1_no_bees_config.json
# Output log:    simulation_results/test1_grid_log.csv
mkdir -p simulation_results
./bin/nectar config/tests/test1_no_bees_config.json simulation_results/test1_grid_log.csv
awk -F';' 'NR<=2 || ($3 ~ /^\([0-9]+,[0-9]+\)$/ && $5 ~ /^<[^,]+,[^,]+,[^,]+,[^,]+,[^,]+,[^,]+,[^,]+,[^,]+>$/) { print }' "simulation_results/test1_grid_log.csv" > "simulation_results/test1_grid_log.csv.tmp" && mv -f "simulation_results/test1_grid_log.csv.tmp" "simulation_results/test1_grid_log.csv"
