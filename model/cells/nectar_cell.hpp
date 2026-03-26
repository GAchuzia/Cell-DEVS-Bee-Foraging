#ifndef NECTAR_CELL
#define NECTAR_CELL

#include <cmath>
#include <algorithm>
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
    [[nodiscard]] nectarState localComputation(
        nectarState state,
        const std::unordered_map<std::vector<int>, NeighborData<nectarState, double>>& neighborhood
    ) const override {

        nectarState newState = state;

        double nectar_regrowth = 1.0;
        double nectar_decay    = 0.05;
        double nectar_consumption = 0.3;

        double pollen_regrowth = 1.0;
        double pollen_decay    = 0.05;

        double pollen_per_bee_visit = 2.0;

        double max_nectar = 100.0;
        double max_pollen = 50.0;
        int    max_bees   = 50;

        // bee movement

        // Find best neighbor resource (nectar + pollen)
        double best_neighbor_resource = 0.0;
        for (const auto& [id, n] : neighborhood) {
            double resource = n.state->nectar_lvl + n.state->pollen_lvl;
            best_neighbor_resource = std::max(best_neighbor_resource, resource);
        }

        double current_resource = state.nectar_lvl + state.pollen_lvl;

        // Departure logic 
        int departing_bees = 0;
        bool resources_low = (state.nectar_lvl < 8.0 || state.pollen_lvl < 8.0);
        bool better_elsewhere = (best_neighbor_resource > current_resource * 1.5);

        if (resources_low) {
            departing_bees = static_cast<int>(state.bees * 0.4);
        } else if (better_elsewhere) {
            departing_bees = state.bees / 2;
        }

        // Incoming bees (biased by nectar difference)
        int incoming_bees = 0;
        for (const auto& [neighborId, neighborData] : neighborhood) {
            if (neighborId[0] == 0 && neighborId[1] == 0) continue;

            double nectar_diff = state.nectar_lvl - neighborData.state->nectar_lvl;

            if (nectar_diff > 0) {
                incoming_bees += static_cast<int>(
                    neighborData.state->bees * (0.2 + 0.02 * nectar_diff)
                );
            }
        }

        // Update bee count with cap
        int new_bees = (state.bees - departing_bees) + incoming_bees;
        newState.bees = std::clamp(new_bees, 0, max_bees);

        // nectar dynamics 

        // Nectar regrowth depends on pollen
        if (state.nectar_lvl < max_nectar) {
            newState.nectar_lvl += nectar_regrowth * (1.0 + 0.01 * state.pollen_lvl);
        }

        // Decay
        newState.nectar_lvl -= nectar_decay * state.nectar_lvl;

        // Saturating consumption 
        double nectar_used = (nectar_consumption * newState.bees * state.nectar_lvl)
                           / (1.0 + newState.bees);

        newState.nectar_lvl -= nectar_used;

        // Clamp nectar
        newState.nectar_lvl = std::clamp(newState.nectar_lvl, 0.0, max_nectar);

        // Incoming pollen from neighbors
        double pollen_in = 0.0;
        for (const auto& [neighborId, neighborData] : neighborhood) {
            if (neighborId[0] == 0 && neighborId[1] == 0) continue;

            pollen_in += neighborData.state->bees * pollen_per_bee_visit * 0.025;
        }

        // Outgoing pollen due to departing bees
        double pollen_out = departing_bees * pollen_per_bee_visit * 0.1;

        // Update pollen
        newState.pollen_lvl += pollen_in;
        newState.pollen_lvl -= pollen_out;
        newState.pollen_lvl -= pollen_decay * state.pollen_lvl;

        // Optional mild regrowth (keep from original)
        if (state.pollen_lvl < max_pollen) {
            newState.pollen_lvl += pollen_regrowth * 0.1;
        }

        // Clamp pollen
        newState.pollen_lvl = std::clamp(newState.pollen_lvl, 0.0, max_pollen);

        return newState;
    }

    // Output delay
    [[nodiscard]] double outputDelay(const nectarState& state) const override {
        return 1.0;
    }
};

#endif