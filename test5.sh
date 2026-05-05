#!/bin/bash
# test5.sh - Multi-species reference: wrapped dual-species baseline with five bees and two butterflies.
# Input config:  config/tests/test5_multi_species_config.json
# Output log:    simulation_results/test5_multi_species_grid_log.csv
mkdir -p simulation_results
./bin/nectar config/tests/test5_multi_species_config.json simulation_results/test5_multi_species_grid_log.csv
awk -F';' 'NR<=2 || ($3 ~ /^\([0-9]+,[0-9]+\)$/ && $5 ~ /^<[^,]+,[^,]+,[^,]+,[^,]+,[^,]+,[^,]+,[^,]+,[^,]+>$/) { print }' "simulation_results/test5_multi_species_grid_log.csv" > "simulation_results/test5_multi_species_grid_log.csv.tmp" && mv -f "simulation_results/test5_multi_species_grid_log.csv.tmp" "simulation_results/test5_multi_species_grid_log.csv"