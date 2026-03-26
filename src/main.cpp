#include <cadmium/simulation/root_coordinator.hpp>
#include <cadmium/simulation/logger/csv.hpp>
#include <cadmium/modeling/celldevs/grid/coupled.hpp>

#include <iostream>
#include <string>

#include "../model/nectar_grid.hpp"
#include "../model/cells/nectar_cell.hpp"


int main(int argc, char* argv[]) {
    using namespace cadmium;
    using namespace cadmium::celldevs;

    std::string configPath = "config/nectarVisualization_config.json";
    std::string logPath = "simulation_results/grid_log.csv";

    if (argc >= 2) {
        configPath = argv[1];
    }
    if (argc >= 3) {
        logPath = argv[2];
    }
    if (argc > 3) {
        std::cerr << "Usage: " << argv[0] << " [config.json] [output_log.csv]\n";
        return 1;
    }

    auto model = std::make_shared<NectarGrid>("nectar", configPath);

    model->buildModel();

    RootCoordinator rootCoordinator(model);

    rootCoordinator.setLogger<CSVLogger>(logPath, ";");

    rootCoordinator.start();
    rootCoordinator.simulate(50.0);
    rootCoordinator.stop();

    return 0;
}