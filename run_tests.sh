#!/bin/bash
# run_tests.sh
# Builds the project, then runs all six bee-foraging test scenarios.
# Each test prints what it expects to observe in the output.
#
# Usage: bash run_tests.sh

# Filters a raw simulation CSV to only cell-coordinate rows (strips agent rows)
# so the Cell-DEVS web viewer can load it correctly.
filter_csv() {
    local f="$1"
    awk -F';' 'NR<=2 || ($3 ~ /^\([0-9]+,[0-9]+\)$/ && $5 ~ /^<[^,]+,[^,]+,[^,]+,[^,]+,[^,]+,[^,]+,[^,]+,[^,]+>$/) { print }' \
        "$f" > "$f.tmp" && mv -f "$f.tmp" "$f"
}

# Build first (matches 100%-grade submission pattern)
source build.sh

mkdir -p simulation_results

echo ""
echo "======================================================================"
echo "Test 1: No pollinators"
echo "  Two plant species and pollen types; zero bees and butterflies."
echo "  Expected: nectar and pollen regenerate steadily with no depletion."
echo "======================================================================"
./bin/nectar config/tests/test1_no_bees_config.json simulation_results/test1_grid_log.csv
filter_csv simulation_results/test1_grid_log.csv
echo "Log: simulation_results/test1_grid_log.csv"

echo ""
echo "======================================================================"
echo "Test 2: Center foragers (unwrapped)"
echo "  Four bees and two butterflies start at center cells."
echo "  Expected: nectar depletion radiates outward from center."
echo "======================================================================"
./bin/nectar config/tests/test2_center_burst_config.json simulation_results/test2_grid_log.csv
filter_csv simulation_results/test2_grid_log.csv
echo "Log: simulation_results/test2_grid_log.csv"

echo ""
echo "======================================================================"
echo "Test 3: Corner foragers — unwrapped boundaries"
echo "  Species-B band on west edge; species-A hotspot at southeast."
echo "  Expected: pollinators confined to their half; no wrap-around."
echo "======================================================================"
./bin/nectar config/tests/test3_corner_unwrapped_config.json simulation_results/test3_unwrapped_grid_log.csv
filter_csv simulation_results/test3_unwrapped_grid_log.csv
echo "Log: simulation_results/test3_unwrapped_grid_log.csv"

echo ""
echo "======================================================================"
echo "Test 4: Wrapped torus boundaries"
echo "  Northwest species-A patch; east species-B band; wrapped=true."
echo "  Expected: pollinators cross grid edges via torus topology."
echo "======================================================================"
./bin/nectar config/tests/test4_corner_wrapped_config.json simulation_results/test4_wrapped_grid_log.csv
filter_csv simulation_results/test4_wrapped_grid_log.csv
echo "Log: simulation_results/test4_wrapped_grid_log.csv"

echo ""
echo "======================================================================"
echo "Test 5: Multi-species reference"
echo "  Dual-species grid; five bees and two butterflies."
echo "  Expected: species-matched pollen (conspecific) accumulates near"
echo "  species-A and species-B patches respectively."
echo "======================================================================"
./bin/nectar config/tests/test5_multi_species_config.json simulation_results/test5_multi_species_grid_log.csv
filter_csv simulation_results/test5_multi_species_grid_log.csv
echo "Log: simulation_results/test5_multi_species_grid_log.csv"

echo ""
echo "======================================================================"
echo "Test 6: River barrier (Asymmetric Cell-DEVS)"
echo "  Row 4 is a full river; cells have empty neighborhoods (isolated)."
echo "  Expected: pollinators and nectar dynamics are fully contained in"
echo "  the upper half (rows 0-3) and lower half (rows 5-9) separately."
echo "======================================================================"
./bin/nectar config/tests/test6_river_config.json simulation_results/test6_river_grid_log.csv
filter_csv simulation_results/test6_river_grid_log.csv
echo "Log: simulation_results/test6_river_grid_log.csv"

echo ""
echo "All tests complete."
echo "Load any config + its log CSV at https://devssim.carleton.ca/cell-devs-viewer/"
