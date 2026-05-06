# Cell-DEVS Bee Foraging

Model and simulation of pollinator foraging dynamics as a hybrid Cell-DEVS and DEVS system on a 10×10 asymmetric cell-space.

## Usage

To build the model, run

```
source build.sh
```

To run the default scenario, run

```
bash run.sh
```

Output is logged to

```
simulation_results/grid_log.csv
```

To run all six test scenarios, run

```
bash run_tests.sh
```

Output for each test is logged to

```
simulation_results/test<N>_grid_log.csv
```

The scripts are:

* `build.sh`: Compiles the project using CMake.
* `run.sh`: Runs the default nectar visualization scenario.
* `run_tests.sh`: Builds and runs all six test scenarios.
* `test1.sh` – `test6.sh`: Run individual test scenarios.
* `generate_asymm_configs.py`: Generates asymmetric Cell-DEVS JSON configs from scenario definitions.

To visualize the output, upload the config and log CSV pair to the __[Cell-DEVS Web Viewer](https://devssim.carleton.ca/cell-devs-viewer/)__.

For a detailed description of the project, see the __[report](Report.pdf)__.

## Test Scenarios

* **Test 1 – No pollinators:** Two plant species with zero bees and butterflies. Nectar and pollen dynamics run undisturbed.
* **Test 2 – Center foragers:** Four bees and two butterflies starting at center cells on an unwrapped grid.
* **Test 3 – Corner foragers (unwrapped):** Species-B band on the west edge and a species-A hotspot at the southeast corner; hard boundaries.
* **Test 4 – Wrapped torus:** Northwest species-A patch and east species-B band on a torus topology.
* **Test 5 – Multi-species reference:** Dual-species grid with five bees and two butterflies.
* **Test 6 – River barrier:** All cells in row 4 are topologically isolated using asymmetric Cell-DEVS neighborhoods, splitting the grid into two fully independent halves.

## Requirements

* __[Cadmium v2](https://github.com/Sasisekhar/cadmium_v2)__

## Further Reading

* __[Asymmetric Cell-DEVS models with the Cadmium Simulator](https://www.sciencedirect.com/science/article/pii/S1569190X22001198)__
* __[Cadmium and the DEVSsim Server](https://devssim.carleton.ca/)__

## License

__[MIT](https://choosealicense.com/licenses/mit/)__