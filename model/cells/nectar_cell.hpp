#ifndef NECTAR_CELL
#define NECTAR_CELL

#include <algorithm>
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
        const int max_bees = 60;

        int incoming_bees = 0;
        for (const auto& [neighborId, neighborData] : neighborhood) {
            incoming_bees += static_cast<int>(neighborData.state->bees * 0.25);
        }
        int new_bees = incoming_bees + (state.bees / 2);
        newState.bees = static_cast<int>(
            std::clamp(static_cast<long long>(new_bees), 0LL, static_cast<long long>(max_bees)));

        // Nectar dynamics
        if (state.nectar_lvl < max_nectar) {
            newState.nectar_lvl += nectar_regrowth;
        }
        newState.nectar_lvl -= nectar_decay * state.nectar_lvl;
        newState.nectar_lvl -= nectar_consumption * newState.bees;

        // Pollen dynamics
        if (state.pollen_lvl < max_pollen) {
            newState.pollen_lvl += pollen_regrowth;
        }
        newState.pollen_lvl -= pollen_decay * state.pollen_lvl;
        newState.pollen_lvl -= pollen_consumption * newState.bees;

        newState.nectar_lvl = std::clamp(newState.nectar_lvl, 0.0, max_nectar);
        newState.pollen_lvl = std::clamp(newState.pollen_lvl, 0.0, max_pollen);

        return newState;
    }

    // Output delay
    [[nodiscard]] double outputDelay(const nectarState& state) const override {
        return 1.0;
    }
};

#endif