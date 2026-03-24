#include <cadmium/simulation/root_coordinator.hpp>
#include <cadmium/simulation/logger/csv.hpp>
#include <cadmium/modeling/celldevs/grid/coupled.hpp>

#include "../model/nectar_grid.hpp"
#include "../model/cells/nectar_cell.hpp"


int main() {
    using namespace cadmium;
    using namespace cadmium::celldevs;

    auto model = std::make_shared<GridCellDEVSCoupled<nectarState, double>>("nectar", addGridCell, "config/initial_scenario.json");
    
    model->buildModel(); 

    RootCoordinator rootCoordinator(model);

    // CSV Logger
    rootCoordinator.setLogger<CSVLogger>("../simulation_results/simulation_output.csv", ";");

    // Run simulation
    rootCoordinator.start();
    rootCoordinator.simulate(50.0); // 50 time units
    rootCoordinator.stop();

    return 0;
}