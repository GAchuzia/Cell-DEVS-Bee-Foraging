#ifndef NECTAR_GRID
#define NECTAR_GRID

#include <iostream>
#include <memory>

#include <cadmium/modeling/devs/port.hpp>
#include <cadmium/modeling/celldevs/asymm/coupled.hpp>

#include "cells/nectar_cell.hpp"

using namespace cadmium;
using namespace cadmium::celldevs;

// ── NectarGrid (Asymmetric Cell-DEVS coupled model) ──────────────────────────
//
// Inherits from AsymmCellDEVSCoupled, which reads the per-cell topology from
// the JSON config (explicit neighborhood maps, no "scenario" block required).
// External ports are added before buildModel() so the coupled model exposes
// them; EIC connections are wired afterward by iterating the components map.
//
class NectarGrid : public cadmium::celldevs::AsymmCellDEVSCoupled<nectarState, double> {
public:
    cadmium::Port<BeeMovement>       in_bee_move;
    cadmium::Port<ButterflyMovement> in_butterfly_move;

    NectarGrid(const std::string& id, const std::string& configPath)
        : AsymmCellDEVSCoupled<nectarState, double>(id, addNectarCell, configPath)
    {
        // Add external input ports before buildModel() so they are registered
        in_bee_move       = addInPort<BeeMovement>("in_bee_move");
        in_butterfly_move = addInPort<ButterflyMovement>("in_butterfly_move");

        // Build the asymmetric cell topology from the JSON config
        this->buildModel();

        // Wire the grid-level ports to every individual cell's input ports.
        // After buildModel(), this->components maps each cell ID string to its
        // shared_ptr<Component>; we cast each to NectarCell to access its ports.
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
    // Factory function required by AsymmCellDEVSCoupled.
    // Returns a NectarCell for every cell ID found in the JSON config.
    static std::shared_ptr<AsymmCell<nectarState, double>> addNectarCell(
        const std::string& cellId,
        const std::shared_ptr<const AsymmCellConfig<nectarState, double>>& cellConfig)
    {
        return std::make_shared<NectarCell>(cellId, cellConfig);
    }
};

#endif // NECTAR_GRID
