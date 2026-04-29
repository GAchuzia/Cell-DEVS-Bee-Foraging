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
        if (!file.is_open()) {
            throw std::runtime_error("Cannot open config file: " + configPath);
        }
        try {
            file >> j;
        } catch (const nlohmann::json::exception& e) {
            throw std::runtime_error("Failed to parse JSON config '" + configPath + "': " + std::string(e.what()));
        }
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

    auto printRuntimeHint = [&](const std::string& err) {
        std::cerr << "Simulation failed: " << err << std::endl;
        std::cerr << "Config: " << configPath << std::endl;
        std::cerr << "Output CSV target: " << logPath << std::endl;
        if (err.find("component ID already defined") != std::string::npos) {
            std::cerr << "Hint: duplicate component IDs were detected while building the model." << std::endl;
            std::cerr << "Check that you rebuilt after updates (`make clean && make`) and that agent components use unique names." << std::endl;
            std::cerr << "If editing config, keep each coordinate entry valid in initial_agents/initial_butterflies." << std::endl;
        }
    };

    std::shared_ptr<BeeSimulation> model;
    try {
        model = std::make_shared<BeeSimulation>("nectar", configPath);
    } catch (const std::exception& e) {
        printRuntimeHint(e.what());
        return 1;
    } catch (...) {
        printRuntimeHint("unknown error while constructing the simulation model");
        return 1;
    }

    try {
        auto rootCoordinator = RootCoordinator(model);
        rootCoordinator.setLogger<CSVLogger>(logPath, ";");

        rootCoordinator.start();
        rootCoordinator.simulate(50.0);
        rootCoordinator.stop();
    } catch (const std::exception& e) {
        printRuntimeHint(e.what());
        return 1;
    } catch (...) {
        printRuntimeHint("unknown error while running the coordinator");
        return 1;
    }
    std::cout << "Simulation completed successfully." << std::endl;
    std::cout << "Config used: " << configPath << std::endl;
    std::cout << "CSV log written to: " << logPath << std::endl;
    std::cout << "Direct ./nectar output is raw Cadmium CSV." << std::endl;
    std::cout << "For viewer-ready filtered CSV, run via Makefile targets (make run / make testN)." << std::endl;
    std::cout << "Raw pair generated: " << configPath << " + " << logPath << std::endl;

    return 0;
}