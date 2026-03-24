#ifndef NECTAR_GRID
#define NECTAR_GRID

#include <cadmium/modeling/celldevs/grid/coupled.hpp>
#include "cells/nectar_cell.hpp"

using namespace cadmium::celldevs;

class NectarGrid : public GridCellDEVSCoupled<nectarState, double> {
public:
    NectarGrid(std::string const &id, std::string const &configPath) 
        : GridCellDEVSCoupled<nectarState, double>(id, addNectarCell, configPath) {}

private:
    static std::shared_ptr<GridCell<nectarState, double>> addNectarCell(
        const coordinates &cellId, 
        const std::shared_ptr<const GridCellConfig<nectarState, double>>& cellConfig) 
    {
        if (cellConfig->cellModel == "nectar") {
            return std::make_shared<NectarCell>(cellId, cellConfig);
        }
        throw std::runtime_error("Unknown cell model: " + cellConfig->cellModel);
    }
};

#endif