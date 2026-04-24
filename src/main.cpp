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
#include "../model/butterfly.hpp"

using namespace cadmium;

class BeeSimulation: public Coupled {
public:
    
    BeeSimulation(const std::string& id, const std::string& configPath) : Coupled(id) {
        
        std::ifstream file(configPath);
        nlohmann::json j;
        file >> j;
        auto grid = std::make_shared<NectarGrid>("nectar_grid", configPath);

        this->addComponent(grid);
        int nextId = 1;

        // Dynamically create and route bees
        if (j["cells"].contains("initial_agents") && j["cells"]["initial_agents"].contains("cell_map")) {
        auto beePositions = j["cells"]["initial_agents"]["cell_map"].get<std::vector<std::vector<int>>>();
        
        for (const auto& pos : beePositions) {
            auto bee = std::make_shared<Bee>(nextId++, Role::FORAGER, pos);
            this->addComponent(bee);
            this->addIC(bee->out, grid->in_bee_move);
        }
        std::cout << "Instantiated " << beePositions.size() << " bees from config." << std::endl;
     } 

        // 4. Dynamically create and route Butterflies
        if (j["cells"].contains("initial_butterflies") && j["cells"]["initial_butterflies"].contains("cell_map")) {
        auto butterflyPositions = j["cells"]["initial_butterflies"]["cell_map"].get<std::vector<std::vector<int>>>();
        
        for (const auto& pos : butterflyPositions) {
            auto butterfly = std::make_shared<Butterfly>(nextId++, pos);
            this->addComponent(butterfly);
            this->addIC(butterfly->out, grid->in_butterfly_move);

        }
        std::cout << "Instantiated " << butterflyPositions.size() << " butterflies from config." << std::endl;
    }
        
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