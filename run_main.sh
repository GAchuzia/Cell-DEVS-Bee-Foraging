#!/bin/bash
# run.sh - Runs the default bee foraging simulation.
# Input config:  config/nectarVisualization_config.json
# Output log:    simulation_results/grid_log.csv
#
# Usage: source build_sim.sh  (first time, to compile)
#        bash run.sh           (to run the default scenario)
mkdir -p simulation_results
./bin/nectar config/nectarVisualization_config.json simulation_results/grid_log.csv
awk -F';' 'NR<=2 || ($3 ~ /^\([0-9]+,[0-9]+\)$/ && $5 ~ /^<[^,]+,[^,]+,[^,]+,[^,]+,[^,]+,[^,]+,[^,]+,[^,]+>$/) { print }' "simulation_results/grid_log.csv" > "simulation_results/grid_log.csv.tmp" && mv -f "simulation_results/grid_log.csv.tmp" "simulation_results/grid_log.csv"