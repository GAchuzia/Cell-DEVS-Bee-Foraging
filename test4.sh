#!/bin/bash
# test4.sh - Wrapped torus: northwest species-A patch and east species-B band on a torus (wrapped: true).
# Input config:  config/tests/test4_corner_wrapped_config.json
# Output log:    simulation_results/test4_wrapped_grid_log.csv
mkdir -p simulation_results
./bin/nectar config/tests/test4_corner_wrapped_config.json simulation_results/test4_wrapped_grid_log.csv
