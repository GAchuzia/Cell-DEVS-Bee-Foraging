# Cell-DEVS-Bee-Foraging

Cadmium Cell-DEVS model of bee foraging on a 10×10 grid. 

Each cell is a nectar patch with nectar, pollen, and bees. Neighbor coupling uses a Von Neumann neighborhood.

---

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

---

## Layout

| Path | Role |
|------|------|
| `src/main.cpp` | Builds `nectar` |
| `model/nectar_grid.hpp` | Coupled Cell-DEVS grid |
| `model/cells/nectar_cell.hpp` | Local transition \(\tau\) |
| `model/cells/nectarState.hpp` | State struct and JSON parsing |
| `config/nectarVisualization_config.json` | Default demo (center high-activity block) |
| `config/tests/*_config.json` | Experimental-frame scenarios (tests 1–4) |
| `simulation_results/` | CSV logs (and any `.webm` recordings you add locally) |

---

## Build

From the repository root (adjust paths for your machine):

```bash
make
# Optional overrides:
# make CADMIUM_PATH=/path/to/cadmium/include JSON_PATH=/path/to/json/include
```

Produces the `nectar` executable.

---

## Run

```bash
mkdir -p simulation_results

# Defaults: config/nectarVisualization_config.json and simulation_results/grid_log.csv
./nectar

# Explicit paths:
./nectar config/tests/test1_no_bees_config.json simulation_results/test1_grid_log.csv
```

The simulation horizon is **50** time units.

---

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
| `make tests` | runs `test1`–`test4` in order | N/A |


