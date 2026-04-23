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

    //     // Create bee starting at position (4,4)
    //     std::vector<int> startPos = {4, 4};
    //     auto bee1 = std::make_shared<Bee>(1, Role::FORAGER, startPos);
    //     auto bee2 = std::make_shared<Bee>(2, Role::SCOUT, std::vector<int>{2, 2});
    //     auto bee3 = std::make_shared<Bee>(3, Role::SCOUT, std::vector<int>{5, 4});

        
    //     auto butterfly1 = std::make_shared<Butterfly>(1, std::vector<int>{7, 7});
    //     auto butterfly2 = std::make_shared<Butterfly>(2, std::vector<int>{8, 9});

    //     this->addComponent(grid);
    //     this->addComponent(bee1);
    //     this->addComponent(bee2);
    //     this->addComponent(bee3);

    //     this->addComponent(butterfly1);
    //             this->addComponent(butterfly2);


    //     // Connect bee output to grid input port
    //     this->addIC(bee1->getId(), "out", grid->getId(), "in_bee_move");
    //     this->addIC(bee2->getId(), "out", grid->getId(), "in_bee_move");
    //     this->addIC(bee3->getId(), "out", grid->getId(), "in_bee_move");
    //    // this->addIC(bee4->getId(), "out", grid->getId(), "in_bee_move");

    //     this->addIC(butterfly1->getId(), "out", grid->getId(), "in_butterfly_move");
    //             this->addIC(butterfly2->getId(), "out", grid->getId(), "in_butterfly_move");

        this->addComponent(grid);

        // 3. Dynamically create and route Bees
        if (j.contains("agents") && j["agents"].contains("bees")) {
            for (const auto& b : j["agents"]["bees"]) {
                int beeId = b["id"];
                std::vector<int> pos = b["pos"];
                std::string roleStr = b["role"];
                
                // Map string to Role enum
                Role role = Role::SCOUT; 
                if (roleStr == "FORAGER") role = Role::FORAGER;
                else if (roleStr == "NURSE") role = Role::NURSE;

                auto bee = std::make_shared<Bee>(beeId, role, pos);
                this->addComponent(bee);
                this->addIC(bee->out, grid->in_bee_move);
            }
        }

        // 4. Dynamically create and route Butterflies
        if (j.contains("agents") && j["agents"].contains("butterflies")) {
            for (const auto& b : j["agents"]["butterflies"]) {
                int butId = b["id"];
                std::vector<int> pos = b["pos"];

                auto butterfly = std::make_shared<Butterfly>(butId, pos);
                this->addComponent(butterfly);
                this->addIC(butterfly->out, grid->in_butterfly_move);
            }
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