# Cell-DEVS-Bee-Foraging

Cadmium Cell-DEVS model of bee foraging on a 10×10 grid.

Each cell is a nectar patch with nectar, pollen, and bees. Neighbor coupling uses a Von Neumann neighborhood.

## Requirements

- C++17 compiler (`g++`)
- [Cadmium](https://github.com/SimulationEverywhere/cadmium) headers on the include path
- [nlohmann/json](https://github.com/nlohmann/json) on the include path. Example on Linux/macOS:

  ```bash
  mkdir -p ~/libs/nlohmann
  cd ~/libs/nlohmann
  wget https://github.com/nlohmann/json/releases/download/v3.11.2/json.hpp
  ```
  
  Then point `JSON_PATH` at the parent of `nlohmann` (e.g. `make JSON_PATH=$HOME/libs` so `#include <nlohmann/json.hpp>` resolves).

## Layout

| Path | Role |
|------|------|
| `src/main.cpp` | Builds `nectar` |
| `model/nectar_grid.hpp` | Coupled Cell-DEVS grid |
| `model/cells/nectar_cell.hpp` | Local transition \(\tau\) |
| `model/cells/nectarState.hpp` | State struct and JSON parsing |
| `config/nectarVisualization_config.json` | Default demo (center high-activity block) |
| `config/tests/*_config.json` | Test scenarios (tests 1–6) |
| `simulation_results/` | CSV logs and `.webm` recordings |
| `assets/test*.gif` | Short GIF previews for the README (see **Test scenario previews** below) |

## Build

From the repository root:

**Step 1:** Open a terminal in the project root.

```bash
cd /path/to/Cell-DEVS-Bee-Foraging
```

**Step 2:** Ensure dependencies exist:

- `g++` with C++17 support
- Cadmium headers at `/home/cadmium/rt_cadmium/include` (default server path)
- `nlohmann/json.hpp` under `$HOME/libs/nlohmann/json.hpp` (default user path)

**Step 3:** Build.

```bash
make
```

`Makefile` defaults:

- `CADMIUM_PATH=/home/cadmium/rt_cadmium/include`
- `JSON_PATH=$(HOME)/libs` (parent directory containing `nlohmann/`)

If your paths differ, override them at build time:

```bash
make CADMIUM_PATH=/absolute/path/to/rt_cadmium/include JSON_PATH=/absolute/path/to/parent/of/nlohmann
```

**Step 4:** Confirm build output:

```bash
ls nectar
```

This produces the `nectar` executable.

## Run (Use Make Targets)

```bash
# Always run scenarios through make targets:
make run
make test1
make tests
```

Do **not** use `./nectar` for normal runs. The expected workflow is `make run` / `make testN` so logs are filtered for the Cell-DEVS web viewer.

For visualization/reporting, the input config and output CSV are a pair:
- Main/default result pair: `config/nectarVisualization_config.json` + `simulation_results/grid_log.csv`
- All other `config/tests/*_config.json` with their `simulation_results/test*_grid_log.csv` outputs are test runs (tests 1-6)

`./nectar` is only for advanced/manual debugging. It writes a raw Cadmium CSV that is not the recommended submission/viewer format.

The simulation horizon is **50** time units.

## Make targets

| Target | Config | Log output / effect |
|--------|--------|---------------------|
| `make`  | N/A | Compiles `./nectar` |
| `make clean` | — | Removes the `nectar` binary only (does not delete CSV logs under `simulation_results/`) |
| `make run` | `config/nectarVisualization_config.json` | `simulation_results/grid_log.csv` |
| `make test1` | `config/tests/test1_no_bees_config.json` | `simulation_results/test1_grid_log.csv` |
| `make test2` | `config/tests/test2_center_burst_config.json` | `simulation_results/test2_grid_log.csv` |
| `make test3` | `config/tests/test3_corner_unwrapped_config.json` | `simulation_results/test3_unwrapped_grid_log.csv` |
| `make test4` | `config/tests/test4_corner_wrapped_config.json` | `simulation_results/test4_wrapped_grid_log.csv` |
| `make test5` | `config/tests/test5_multi_species_config.json` | `simulation_results/test5_multi_species_grid_log.csv` |
| `make test6` | `config/tests/test6_river_config.json` | `simulation_results/test6_river_grid_log.csv` |
| `make tests` | runs `test1`–`test6` in order | N/A (writes each test’s CSV under `simulation_results/`) |

`make run` and each `make testN` also filter the produced CSV in place for the Cell-DEVS web viewer (see `FILTER_LOG_FOR_WEB_VIEWER` in the `Makefile`).

## Test Scenarios

Screen captures from the Cell-DEVS web viewer (GIFs under `assets/`).  

In these scenarios, butterflies are modeled with diagonal movement to highlight theiir less efficient exploration compared to bees.

**Note:** The play button does not always show the correct visualization in the DEVS Web Viewer, so use the slider to view panel updates. You can also see preview GIFs in the [`assets/`](./assets/) folder.

**Test 1: no pollinators**  
Two plant species and pollen types, with zero bees and butterflies.

![Test 1: no pollinators](assets/test1_no_pollinators.gif)

**Test 2: center foragers**  
Four center bees and two butterflies on an unwrapped grid.

![Test 2: center foragers](assets/test2_center_foragers.gif)

**Test 3: unwrapped boundaries**  
West species-B band plus southeast species-A hotspot on hard edges (`wrapped: false`).

![Test 3: unwrapped boundaries](assets/test3_unwrapped_boundaries.gif)

**Test 4: wrapped torus**  
Northwest species-A patch and east species-B band on a torus (`wrapped: true`).

![Test 4: wrapped torus](assets/test4_wrapped_boundaries.gif)

**Test 5: general multi-species reference**  
Wrapped dual-species baseline with five bees and two butterflies.

![Test 5: multi-species reference](assets/test5_multi_species.gif)

**Test 6: river barrier**  
A full river row splits the grid and constrains pollinator movement.   
**Note:** This test is based on Asymmetric Cell-DEVS with a river row obstacle.

![Test 6: river barrier](assets/test6_river_barrier.gif)
