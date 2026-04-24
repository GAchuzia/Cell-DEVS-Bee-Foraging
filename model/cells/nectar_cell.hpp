#ifndef NECTAR_CELL
#define NECTAR_CELL

#include <cmath>
#include <algorithm>
#include <vector>
#include <nlohmann/json.hpp>

#include <cadmium/modeling/devs/port.hpp> 
#include <cadmium/modeling/celldevs/grid/cell.hpp>
#include <cadmium/modeling/celldevs/grid/config.hpp>

#include "nectarState.hpp"

struct BeeMovement {
    int bee_id;
    int x, y;
    int action; // 0: leaving, 1: arriving, 2: staying
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
    int action; // 0: leaving, 1: arriving, 2: staying
    double consumption_request;
    int pollen_type;
};

inline std::ostream& operator<<(std::ostream& os, const ButterflyMovement& m) {
    os << "<0,0,0>";
    return os;
}

using namespace cadmium::celldevs;

class NectarCell : public cadmium::celldevs::GridCell<nectarState, double> {
public:
    cadmium::Port<BeeMovement> in_bee_event;
    cadmium::Port<ButterflyMovement> in_butterfly_event;
    int x;
    int y;

    // Constructor
    NectarCell(const cadmium::celldevs::coordinates& id,
               const std::shared_ptr<const cadmium::celldevs::GridCellConfig<nectarState, double>>& config)
    : GridCell<nectarState, double>(id, config) { 
        x = id[0];
        y = id[1];
        in_bee_event = addInPort<BeeMovement>("in_bee_event");
        in_butterfly_event = addInPort<ButterflyMovement>("in_butterfly_event");
    }


    // External Transition
    //  Triggered when bee sends a consumption request
    void externalTransition(double e) override {
        bool stateChanged = false;

        for (auto const& beeMov : in_bee_event->getBag()) {
            stateChanged = true;
           if (beeMov.x == x && beeMov.y == this->y) {
            if (beeMov.action == 1) {
                state.bees++;
            } else if (beeMov.action == 0) {
                state.bees = std::max(0, state.bees - 1);
            }

            if (beeMov.consumption_request > 0) {
                state.nectar_lvl -= beeMov.consumption_request;
                state.pollen_lvl += (beeMov.consumption_request * 0.12);
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
    }

        for (auto const& butterflyMov : in_butterfly_event->getBag()) {   
            stateChanged = true;   
            if (butterflyMov.x == x && butterflyMov.y == y) {    
            if (butterflyMov.action == 1) {
                state.butterflies++;
            } else if (butterflyMov.action == 0) {
                state.butterflies = std::max(0, state.butterflies - 1);
            }

            if (butterflyMov.consumption_request > 0) {
                state.nectar_lvl -= butterflyMov.consumption_request;
                state.pollen_lvl += (butterflyMov.consumption_request * 0.28);
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
    }

    // process neighborhood
        // for (const auto& msg : inputNeighborhood->getBag()) {
        //     neighborhood.at(msg->cellId).state = msg->state;
        // }

        // // competition-inspired movement as counts (parallel to hybrid agents)
        // double bestNeighborNectar = state.nectar_lvl;
        // double bestNeighborResource = state.nectar_lvl + state.pollen_lvl;
        // for (const auto& [neighborId, neighborData] : neighborhood) {
        //     if (neighborId[0] == 0 && neighborId[1] == 0) continue;
        //     bestNeighborNectar = std::max(bestNeighborNectar, neighborData.state->nectar_lvl);
        //     bestNeighborResource = std::max(
        //         bestNeighborResource,
        //         neighborData.state->nectar_lvl + neighborData.state->pollen_lvl
        //     );
        // }

        // if (bestNeighborResource > (state.nectar_lvl + state.pollen_lvl) * 1.5) {
        //     int movingBees = static_cast<int>(state.bees * 0.4);
        //     state.bees = std::max(0, state.bees - movingBees);
        // }

        // if (bestNeighborNectar > state.nectar_lvl) {
        //     int movingButterflies = static_cast<int>(state.butterflies * 0.4);
        //     state.butterflies = std::max(0, state.butterflies - movingButterflies);
        // }

        // compute next state
        // auto nextState = localComputation(state, neighborhood);
    
        // state = nextState;
        // state.nectar_lvl = std::clamp(state.nectar_lvl, 0.0, 100.0);
        // state.pollen_lvl = std::clamp(state.pollen_lvl, 0.0, 50.0);
        // state.bees = std::clamp(state.bees, 0, 50);
        // state.butterflies = std::clamp(state.butterflies, 0, 60);
        // state.conspecific_pollen = std::clamp(state.conspecific_pollen, 0.0, 1000.0);
        // state.heterospecific_pollen = std::clamp(state.heterospecific_pollen, 0.0, 1000.0);

        if (stateChanged) {
            this->sigma = 0;
        } 
    }

    // Local Computation
    [[nodiscard]] nectarState localComputation(
        nectarState state,
        const std::unordered_map<coordinates, NeighborData<nectarState, double>>& neighborhood) 
    const override {

        nectarState newState = state;

        double nectar_regrowth = 1.0;
        double nectar_decay    = 0.05;
        double nectar_consumption = 0.24;
        double butterfly_consumption = 0.36;

        double pollen_regrowth = 1.0;
        double pollen_decay    = 0.05;

        double max_nectar = 100.0;
        double max_pollen = 50.0;
        int    max_bees   = 50;
        
        double nectar_regen_boost = 0.0;
       
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

        newState.nectar_lvl = std::clamp(newState.nectar_lvl, 0.0, max_nectar);
        newState.pollen_lvl = std::clamp(newState.pollen_lvl, 0.0, max_pollen);

        // Light pollen type decay to emulate washout after visits.
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