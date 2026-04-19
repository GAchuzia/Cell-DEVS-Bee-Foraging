#include <cadmium/simulation/root_coordinator.hpp>
#include <cadmium/simulation/logger/csv.hpp>
#include <cadmium/modeling/celldevs/grid/coupled.hpp>
#include <cadmium/modeling/devs/coupled.hpp>

#include <iostream>
#include <string>
#include <memory>

#include "../model/nectar_grid.hpp"
#include "../model/cells/nectar_cell.hpp"
#include "../model/bee.hpp"

using namespace cadmium;

class BeeSimulation: public Coupled {
public:
    
    BeeSimulation(const std::string& id, const std::string& configPath) : Coupled(id) {
        auto grid = std::make_shared<NectarGrid>("nectar_grid", configPath);

        // Create bee starting at position (4,4)
        std::vector<int> startPos = {4, 4};
        auto bee1 = std::make_shared<Bee>(1, Role::FORAGER, startPos);

        this->addComponent(grid);
        this->addComponent(bee1);

        // Connect bee output to grid input port
        this->addIC(bee1->getId(), "out", grid->getId(), "in_bee_move");
    }

};



int main(int argc, char* argv[]) {

    std::string configPath = "config/tests/test5.json";
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

    auto model = std::make_shared<BeeSimulation>("nectar", configPath);


    auto rootCoordinator = RootCoordinator(model);
    rootCoordinator.setLogger<CSVLogger>(logPath, ";");

    rootCoordinator.start();
    rootCoordinator.simulate(50.0);
    rootCoordinator.stop();

    return 0;
}