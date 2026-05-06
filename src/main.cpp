#include <cadmium/simulation/root_coordinator.hpp>
#include <cadmium/simulation/logger/csv.hpp>
#include <cadmium/modeling/celldevs/asymm/coupled.hpp>
#include <cadmium/modeling/devs/coupled.hpp>

#include <iostream>
#include <fstream>
#include <string>
#include <memory>

#include "../model/nectar_grid.hpp"
#include "../model/cells/nectar_cell.hpp"
#include "../model/bee.hpp"
#include "../model/butterfly.hpp"

using namespace cadmium;

// ── BeeSimulation ─────────────────────────────────────────────────────────────
//
// Top-level coupled model that wires together:
//   • NectarGrid  — Asymmetric Cell-DEVS grid of nectar patches
//   • Bee atomics — one per initial bee position (from "agents" in JSON)
//   • Butterfly atomics — one per initial butterfly position ("butterflies")
//
// Agent positions are stored at the top level of the JSON config (not inside
// "cells") so the Cell-DEVS parser does not try to interpret them as cell IDs.
//
class BeeSimulation : public Coupled {
public:
    BeeSimulation(const std::string& id, const std::string& configPath) : Coupled(id) {

        std::ifstream file(configPath);
        if (!file.is_open()) {
            throw std::runtime_error("Cannot open config file: " + configPath);
        }
        nlohmann::json j;
        try {
            file >> j;
        } catch (const nlohmann::json::exception& e) {
            throw std::runtime_error(
                "Failed to parse JSON config '" + configPath + "': " + std::string(e.what()));
        }

        // Build the asymmetric cell grid
        auto grid = std::make_shared<NectarGrid>("nectar_grid", configPath);
        this->addComponent(grid);

        int nextId = 1;

        // ── Bee agents ────────────────────────────────────────────────────────
        // Positions stored as top-level "agents": [[x,y], [x,y], ...]
        if (j.contains("agents") && j["agents"].is_array()) {
            auto beePositions = j["agents"].get<std::vector<std::vector<int>>>();
            for (const auto& pos : beePositions) {
                auto bee = std::make_shared<Bee>(nextId++, Role::FORAGER, pos);
                this->addComponent(bee);
                this->addIC(bee->out, grid->in_bee_move);
            }
            std::cout << "Instantiated " << beePositions.size() << " bees from config." << std::endl;
        }

        // ── Butterfly agents ──────────────────────────────────────────────────
        // Positions stored as top-level "butterflies": [[x,y], [x,y], ...]
        if (j.contains("butterflies") && j["butterflies"].is_array()) {
            auto bfPositions = j["butterflies"].get<std::vector<std::vector<int>>>();
            for (const auto& pos : bfPositions) {
                auto butterfly = std::make_shared<Butterfly>(nextId++, pos);
                this->addComponent(butterfly);
                this->addIC(butterfly->out, grid->in_butterfly_move);
            }
            std::cout << "Instantiated " << bfPositions.size()
                      << " butterflies from config." << std::endl;
        }
    }
};


int main(int argc, char* argv[]) {

    std::string configPath = "config/nectarVisualization_config.json";
    std::string logPath    = "simulation_results/grid_log.csv";

    if (argc >= 2) configPath = argv[1];
    if (argc >= 3) logPath    = argv[2];
    if (argc > 3) {
        std::cerr << "Usage: " << argv[0] << " [config.json] [output_log.csv]\n";
        return 1;
    }

    auto printRuntimeHint = [&](const std::string& err) {
        std::cerr << "Simulation failed: " << err << std::endl;
        std::cerr << "Config:           " << configPath << std::endl;
        std::cerr << "Output CSV target: " << logPath    << std::endl;
        if (err.find("component ID already defined") != std::string::npos) {
            std::cerr << "Hint: duplicate component IDs — check agent IDs are unique.\n";
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
    std::cout << "Config used:       " << configPath << std::endl;
    std::cout << "CSV log written to: " << logPath    << std::endl;

    return 0;
}
