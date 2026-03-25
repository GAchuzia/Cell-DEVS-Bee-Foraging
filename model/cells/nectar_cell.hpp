#ifndef NECTAR_CELL
#define NECTAR_CELL

#include <cmath>
#include <nlohmann/json.hpp>
#include <cadmium/modeling/celldevs/grid/cell.hpp>
#include <cadmium/modeling/celldevs/grid/config.hpp>
#include "nectarState.hpp"

using namespace cadmium::celldevs;


class NectarCell : public GridCell<nectarState, double> {
public:
    // Constructor
    NectarCell(const std::vector<int>& id,
               const std::shared_ptr<const GridCellConfig<nectarState, double>>& config)
    : GridCell<nectarState, double>(id, config) { }

    // Local Computation
    [[nodiscard]] nectarState localComputation(nectarState state,
        const std::unordered_map<std::vector<int>, NeighborData<nectarState, double>>& neighborhood) const override {
        
        nectarState newState = state;
        
        // Ecosystem parameters
        double nectar_regrowth = 1.0;
        double pollen_regrowth = 1.0;
        double nectar_decay = 0.05;
        double pollen_decay = 0.05;

        // Consumption per bee
        double nectar_consumption = 0.3;
        double pollen_consumption = 0.1;

        // Carrying capacity per cell
        double max_nectar = 100.0;
        double max_pollen = 50.0;
        double max_bees = 50.0;

        int incoming_bees = 0;
        for (const auto& [neighborId, neighborData] : neighborhood) {
            if (neighborId[0] == 0 && neighborId[1] == 0) continue;
            incoming_bees += static_cast<int>(neighborData.state->bees * 0.25);
        }
        // Keep half of local bees and add incoming bees
        int new_bees = static_cast<int>(incoming_bees) + (state.bees / 2);

        // Keep num of bees in range
        newState.bees = new_bees;
        if (newState.bees < 0) newState.bees = 0;
        if (newState.bees > max_bees) newState.bees = max_bees;

        // Nectar dynamics
        double nectar_after_growth = state.nectar_lvl + nectar_regrowth;
        if (nectar_after_growth > max_nectar) {
            nectar_after_growth = max_nectar;
        }
        newState.nectar_lvl -= nectar_decay * state.nectar_lvl;
        newState.nectar_lvl -= nectar_consumption * newState.bees;

        // Pollen dynamics
        if (state.pollen_lvl < max_pollen) {
            newState.pollen_lvl += pollen_regrowth;
        }
        newState.pollen_lvl -= pollen_decay * state.pollen_lvl;
        newState.pollen_lvl -= pollen_consumption * newState.bees;

        // Set negative to 0
        if (newState.nectar_lvl < 0) newState.nectar_lvl = 0;
        if (newState.pollen_lvl < 0) newState.pollen_lvl = 0;

        return newState;
    }

    // Output delay
    [[nodiscard]] double outputDelay(const nectarState& state) const override {
        return 1.0;
    }
};

#endif