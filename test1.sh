#!/bin/bash
# test1.sh - No pollinators: two plant species and pollen types, zero bees and butterflies.
# Input config:  config/tests/test1_no_bees_config.json
# Output log:    simulation_results/test1_grid_log.csv
mkdir -p simulation_results
./bin/nectar config/tests/test1_no_bees_config.json simulation_results/test1_grid_log.csv
