#ifndef NECTAR_GRID
#define NECTAR_GRID

#include <iostream>
#include <memory>

#include <cadmium/modeling/devs/port.hpp>
#include <cadmium/modeling/celldevs/asymm/coupled.hpp>

#include "cells/nectar_cell.hpp"

using namespace cadmium;
using namespace cadmium::celldevs;

class NectarGrid : public cadmium::celldevs::AsymmCellDEVSCoupled<nectarState, double> {
public:
    cadmium::Port<BeeMovement>       in_bee_move;
    cadmium::Port<ButterflyMovement> in_butterfly_move;

    NectarGrid(const std::string& id, const std::string& configPath)
        : AsymmCellDEVSCoupled<nectarState, double>(id, addNectarCell, configPath)
    {
        in_bee_move       = addInPort<BeeMovement>("in_bee_move");
        in_butterfly_move = addInPort<ButterflyMovement>("in_butterfly_move");

        this->buildModel();

        int connected = 0;
        for (auto& [cellId, component] : this->components) {
            auto cell = std::dynamic_pointer_cast<NectarCell>(component);
            if (cell) {
                this->addEIC(in_bee_move,       cell->in_bee_event);
                this->addEIC(in_butterfly_move, cell->in_butterfly_event);
                ++connected;
            }
        }
        std::cout << "NectarGrid: " << connected << " cells connected via EIC." << std::endl;
    }

private:
    static std::shared_ptr<AsymmCell<nectarState, double>> addNectarCell(
        const std::string& cellId,
        const std::shared_ptr<const AsymmCellConfig<nectarState, double>>& cellConfig)
    {
        return std::make_shared<NectarCell>(cellId, cellConfig);
    }
};

#endif // NECTAR_GRID
