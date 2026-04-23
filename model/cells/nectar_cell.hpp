#ifndef NECTAR_CELL
#define NECTAR_CELL

#include <cmath>
#include <algorithm>
#include <vector>
#include <nlohmann/json.hpp>

#include <cadmium/modeling/devs/port.hpp> 
#include <cadmium/modeling/celldevs/asymm/cell.hpp>
#include <cadmium/modeling/celldevs/asymm/config.hpp>

#include "nectarState.hpp"

struct BeeMovement {
    int bee_id;
    int x, y;
    bool entering;
    double consumption_request;
    int pollen_type;
};

inline std::ostream& operator<<(std::ostream& os, const BeeMovement& m) {
    os << "<0,0,0>";
    return os;
}

struct ButterflyMovement {
    int butterfly_id;
    int x, y;
    bool entering;
    double consumption_request;
    int pollen_type;
};

inline std::ostream& operator<<(std::ostream& os, const ButterflyMovement& m) {
    os << "<0,0,0>";
    return os;
}

using namespace cadmium::celldevs;

class NectarCell : public cadmium::celldevs::AsymmCell<nectarState, double> {
public:
    cadmium::Port<BeeMovement> bee_port;
    cadmium::Port<ButterflyMovement> butterfly_port;

    // Constructor
    NectarCell(const std::string& id,
           const std::shared_ptr<const AsymmCellConfig<nectarState, double>>& config)
    : AsymmCell<nectarState, double>(id, config) {
        bee_port = addInPort<BeeMovement>("in_bee_event");
        butterfly_port = addInPort<ButterflyMovement>("in_butterfly_event");
    }

        [[nodiscard]] nectarState localComputation(
        nectarState state,
        const std::unordered_map<std::string, NeighborData<nectarState, double>>& neighborhood
    ) const override {

        const std::string currentCellId = this->getId();

        for (const auto& beeMov : bee_port->getBag()) {
            const std::string targetCellId =
                "(" + std::to_string(beeMov.x) + "," + std::to_string(beeMov.y) + ")";

            if (targetCellId != currentCellId) continue;

            if (beeMov.entering) {
                state.bees++;
            } else {
                state.bees = std::max(0, state.bees - 1);
            }

            if (beeMov.consumption_request > 0) {
                state.nectar_lvl -= beeMov.consumption_request;
                state.pollen_lvl += beeMov.consumption_request * 0.12;
            }

            if (beeMov.pollen_type > 0) {
                state.pollen_type = beeMov.pollen_type;
                if (beeMov.pollen_type == state.plant_species) {
                    state.conspecific_pollen += beeMov.consumption_request;
                } else {
                    state.heterospecific_pollen += beeMov.consumption_request;
                }
            }
        }

        for (const auto& butterflyMov : butterfly_port->getBag()) {
            const std::string targetCellId =
                "(" + std::to_string(butterflyMov.x) + "," + std::to_string(butterflyMov.y) + ")";

            if (targetCellId != currentCellId) continue;

            if (butterflyMov.entering) {
                state.butterflies++;
            } else {
                state.butterflies = std::max(0, state.butterflies - 1);
            }

            if (butterflyMov.consumption_request > 0) {
                state.nectar_lvl -= butterflyMov.consumption_request;
                state.pollen_lvl += butterflyMov.consumption_request * 0.28;
            }

            if (butterflyMov.pollen_type > 0) {
                state.pollen_type = butterflyMov.pollen_type;
                if (butterflyMov.pollen_type == state.plant_species) {
                    state.conspecific_pollen += butterflyMov.consumption_request * 0.7;
                } else {
                    state.heterospecific_pollen += butterflyMov.consumption_request * 1.3;
                }
            }
        }

        double bestNeighborNectar = state.nectar_lvl;
        double bestNeighborResource = state.nectar_lvl + state.pollen_lvl;

        for (const auto& [neighborId, neighborData] : neighborhood) {
            bestNeighborNectar = std::max(
                bestNeighborNectar,
                neighborData.state->nectar_lvl
            );

            double weightedResource =
                neighborData.vicinity *
                (neighborData.state->nectar_lvl + neighborData.state->pollen_lvl);

            bestNeighborResource = std::max(bestNeighborResource, weightedResource);
        }

        if (bestNeighborResource > (state.nectar_lvl + state.pollen_lvl) * 1.5) {
            int movingBees = static_cast<int>(state.bees * 0.4);
            state.bees = std::max(0, state.bees - movingBees);
        }

        if (bestNeighborNectar > state.nectar_lvl) {
            int movingButterflies = static_cast<int>(state.butterflies * 0.4);
            state.butterflies = std::max(0, state.butterflies - movingButterflies);
        }

        nectarState newState = state;

        double nectar_regrowth = 1.0;
        double nectar_decay = 0.05;
        double nectar_consumption = 0.24;
        double butterfly_consumption = 0.36;

        double pollen_regrowth = 1.0;
        double pollen_decay = 0.05;

        double max_nectar = 100.0;
        double max_pollen = 50.0;

        double nectar_regen_boost = 0.0;
        if (state.pollen_type == state.plant_species && state.plant_species != 0) {
            nectar_regen_boost = state.pollen_lvl * 0.02;
        }

        if (state.nectar_lvl < max_nectar) {
            newState.nectar_lvl += nectar_regrowth * (1.0 + 0.01 * state.pollen_lvl) + nectar_regen_boost;
        }

        newState.nectar_lvl -= nectar_decay * state.nectar_lvl;
        newState.nectar_lvl -= nectar_consumption * state.bees;
        newState.nectar_lvl -= butterfly_consumption * state.butterflies;
        newState.nectar_lvl = std::clamp(newState.nectar_lvl, 0.0, max_nectar);

        newState.pollen_lvl -= pollen_decay * state.pollen_lvl;
        if (state.pollen_lvl < max_pollen) {
            newState.pollen_lvl += pollen_regrowth * 0.1;
        }
        newState.pollen_lvl = std::clamp(newState.pollen_lvl, 0.0, max_pollen);

        newState.bees = std::clamp(newState.bees, 0, 50);
        newState.butterflies = std::clamp(newState.butterflies, 0, 60);
        newState.conspecific_pollen = std::clamp(newState.conspecific_pollen, 0.0, 1000.0);
        newState.heterospecific_pollen = std::clamp(newState.heterospecific_pollen, 0.0, 1000.0);

        if (newState.pollen_lvl < 0.2) {
            newState.pollen_type = 0;
        }

        return newState;
    }



    // Output delay
    [[nodiscard]] double outputDelay(const nectarState& state) const override {
        return 1.0;
    }
};

#endif