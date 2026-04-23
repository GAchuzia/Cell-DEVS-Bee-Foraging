#ifndef NECTAR_GRID
#define NECTAR_GRID

#include <iostream>
#include <fstream>

#include <cadmium/modeling/devs/port.hpp> 
#include <cadmium/modeling/celldevs/asymm/coupled.hpp>
#include "cells/nectar_cell.hpp"

using namespace cadmium;
using namespace cadmium::celldevs;

class NectarGrid : public AsymmCellDEVSCoupled<nectarState, double> {
public:
    cadmium::Port<BeeMovement> in_bee_move;
    cadmium::Port<ButterflyMovement> in_butterfly_move;
    
    NectarGrid(std::string const &id, std::string const &configPath) 
        : AsymmCellDEVSCoupled<nectarState, double>(id, addNectarCell, configPath) {

        in_bee_move = addInPort<BeeMovement>("in_bee_move");
        in_butterfly_move = addInPort<ButterflyMovement>("in_butterfly_move");

        std::ifstream file(configPath);
        if (!file.is_open()) {
            throw std::runtime_error("Could not open config file: " + configPath);
        }
        nlohmann::json j;
        file >> j;

        this->AsymmCellDEVSCoupled<nectarState, double>::buildModel();

        auto grid_shape = j["scenario"]["shape"].get<std::vector<int>>();

        // Route the grid's input port to each individual cell's input port
        for (int i = 0; i < grid_shape[0]; ++i) {
            for (int j_idx = 0; j_idx < grid_shape[1]; ++j_idx) {
                std::string cellIdStr = "(" + std::to_string(i) + "," + std::to_string(j_idx) + ")";

                try {
                    this->addDynamicEIC("in_bee_move", cellIdStr, "in_bee_event");
                    this->addDynamicEIC("in_butterfly_move", cellIdStr, "in_butterfly_event");
                } catch (const cadmium::CadmiumModelException& e) {
                    std::cout << "Failed to link cell: " << cellIdStr << " - " << e.what() << std::endl;
                }
                               
            }
        }
        std::cout << "Grid initialized with shape: " << grid_shape[0] << "x" << grid_shape[1] << std::endl;
        std::cout << "DEBUG: Grid Ports registered: ";
        for (auto const& p : this->getInPorts()) {
            std::cout << p<< " ";
        }
        std::cout << std::endl;
            
    }

private:
    static std::shared_ptr<AsymmCell<nectarState, double>> addNectarCell(
        const std::string& id,
        const std::shared_ptr<const AsymmCellConfig<nectarState, double>>& config)
    {
        std::cout << "Registering Cell: " << id << std::endl;
        return std::make_shared<NectarCell>(id, config);    
    }
};

#endif