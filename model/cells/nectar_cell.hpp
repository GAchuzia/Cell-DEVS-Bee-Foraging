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
        
        // Rates for nectar
        double regrowth_rate = 0.5;
        double decay_rate = 0.05;
        double consumption_rate = 0.2;

        int incoming_bees = 0;
        for (const auto& [neighborId, neighborData] : neighborhood) {
            incoming_bees += static_cast<int>(neighborData.state->bees * 0.25);
        }
        newState.bees = incoming_bees;

        // Nectar dynamics
        newState.nectar_lvl += regrowth_rate;
        newState.nectar_lvl -= decay_rate * state.nectar_lvl;
        newState.nectar_lvl -= consumption_rate * state.bees;

        // Pollen dynamics
        newState.pollen_lvl += 0.3;
        newState.pollen_lvl -= 0.1 * state.pollen_lvl;

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