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
    bool entering;
    double consumption_request;
};

inline std::ostream& operator<<(std::ostream& os, const BeeMovement& m) {
    os << "<0,0,0>";
    // os << "{\"bee_id\":" << m.bee_id << ",\"entering\":" << (m.entering ? "true" : "false") << "}";
    return os;
}

using namespace cadmium::celldevs;

class NectarCell : public cadmium::celldevs::GridCell<nectarState, double> {
public:
    cadmium::Port<BeeMovement> bee_port;

    // Constructor
    NectarCell(const cadmium::celldevs::coordinates& id,
               const std::shared_ptr<const cadmium::celldevs::GridCellConfig<nectarState, double>>& config)
    : GridCell<nectarState, double>(id, config) { 
        bee_port = addInPort<BeeMovement>("in_bee_event");
    }


    // External Transition
    //  Triggered when bee sends a consumption request
    void externalTransition(double e) override {
        clock += e;
        // sigma -= e;

        bool beeChanged = false;

        // process bee movements
        for (auto const& beeMov : bee_port->getBag()) {
        beeChanged = true; 
        if(beeMov.entering) {
            state.bees++;
        } else {
            state.bees = std::max(0, state.bees - 1);
        }
        
        if(beeMov.consumption_request > 0) {
            state.nectar_lvl -= beeMov.consumption_request;
            state.pollen_lvl += (beeMov.consumption_request * 0.15);
        }
    }

    // process neighborhood
    for (const auto& msg : inputNeighborhood->getBag()) {
        neighborhood.at(msg->cellId).state = msg->state;
    }

    // compute next state 
    auto nextState = localComputation(state, neighborhood);
    
    state = nextState;
    state.nectar_lvl = std::clamp(state.nectar_lvl, 0.0, 100.0);
    state.pollen_lvl = std::clamp(state.pollen_lvl, 0.0, 50.0);

    // trigger an output i state changed
    if (beeChanged || nextState != state) {
        sigma = 0; 
    } else {
        // Continue waiting 
        // sigma = outputQueue->size() == 0 ? std::numeric_limits<double>::infinity() : outputQueue->nextTime() - clock;
        sigma = outputQueue->nextTime() - clock;
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
        double nectar_consumption = 0.3;

        double pollen_regrowth = 1.0;
        double pollen_decay    = 0.05;

        double max_nectar = 100.0;
        double max_pollen = 50.0;
        int    max_bees   = 50;
        
        // nectar dynamics 

        // Nectar regrowth depends on pollen
        if (state.nectar_lvl < max_nectar) {
            newState.nectar_lvl += nectar_regrowth * (1.0 + 0.01 * state.pollen_lvl);
        }

        // Decay
        newState.nectar_lvl -= nectar_decay * state.nectar_lvl;

        // Clamp nectar
        newState.nectar_lvl = std::clamp(newState.nectar_lvl, 0.0, max_nectar);

        // Incoming pollen from neighbors
        // double pollen_in = 0.0;
        // for (const auto& [neighborId, neighborData] : neighborhood) {
        //     if (neighborId[0] == 0 && neighborId[1] == 0) continue;
        //     pollen_in += neighborData.state->bees * pollen_per_bee_visit * 0.025;
        // }

        // // Outgoing pollen due to departing bees
        // double pollen_out = departing_bees * pollen_per_bee_visit * 0.1;

        // Update pollen
        // newState.pollen_lvl += pollen_in;
        // newState.pollen_lvl -= pollen_out;
        newState.pollen_lvl -= pollen_decay * state.pollen_lvl;

        // Optional mild regrowth
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