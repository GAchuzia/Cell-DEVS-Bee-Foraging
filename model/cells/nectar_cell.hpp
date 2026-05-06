#ifndef NECTAR_CELL
#define NECTAR_CELL

#include <cmath>
#include <algorithm>
#include <cstdio>
#include <vector>
#include <nlohmann/json.hpp>

#include <cadmium/modeling/devs/port.hpp>
#include <cadmium/modeling/celldevs/asymm/cell.hpp>
#include <cadmium/modeling/celldevs/asymm/config.hpp>

#include "nectarState.hpp"

struct BeeMovement {
    int bee_id;
    int x, y;
    int action;              // 0: leaving, 1: arriving, 2: staying
    double consumption_request;
    int pollen_type;
};

inline std::ostream& operator<<(std::ostream& os, const BeeMovement&) {
    os << "<0,0,0>";
    return os;
}

struct ButterflyMovement {
    int butterfly_id;
    int x, y;
    int action;              // 0: leaving, 1: arriving, 2: staying
    double consumption_request;
    int pollen_type;
};

inline std::ostream& operator<<(std::ostream& os, const ButterflyMovement&) {
    os << "<0,0,0>";
    return os;
}

using namespace cadmium::celldevs;

class NectarCell : public cadmium::celldevs::AsymmCell<nectarState, double> {
public:
    cadmium::Port<BeeMovement>       in_bee_event;
    cadmium::Port<ButterflyMovement> in_butterfly_event;

    int cell_x;
    int cell_y;

    NectarCell(const std::string& id,
               const std::shared_ptr<const cadmium::celldevs::AsymmCellConfig<nectarState, double>>& config)
        : AsymmCell<nectarState, double>(id, config)
    {
        if (sscanf(id.c_str(), "(%d,%d)", &cell_x, &cell_y) != 2) {
            cell_x = 0;
            cell_y = 0;
        }
        in_bee_event       = addInPort<BeeMovement>("in_bee_event");
        in_butterfly_event = addInPort<ButterflyMovement>("in_butterfly_event");
        this->sigma = 1.0;
    }

    void externalTransition(double /*e*/) override {
        bool stateChanged = false;

        for (const auto& beeMov : in_bee_event->getBag()) {
            if (beeMov.x != cell_x || beeMov.y != cell_y) continue;
            stateChanged = true;

            if (beeMov.action == 1) {           // arriving
                state.bees++;
            } else if (beeMov.action == 0) {    // leaving
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

        for (const auto& bfMov : in_butterfly_event->getBag()) {
            if (bfMov.x != cell_x || bfMov.y != cell_y) continue;
            stateChanged = true;

            if (bfMov.action == 1) {            // arriving
                state.butterflies++;
            } else if (bfMov.action == 0) {     // leaving
                state.butterflies = std::max(0, state.butterflies - 1);
            }

            if (bfMov.consumption_request > 0) {
                state.nectar_lvl -= bfMov.consumption_request;
                state.pollen_lvl += bfMov.consumption_request * 0.28;
            }

            if (bfMov.pollen_type > 0) {
                state.pollen_type = bfMov.pollen_type;
                if (bfMov.pollen_type == state.plant_species) {
                    state.conspecific_pollen    += bfMov.consumption_request * 0.7;
                } else {
                    state.heterospecific_pollen += bfMov.consumption_request * 1.3;
                }
            }
        }

        if (stateChanged) {
            this->sigma = 0;
        }
    }

    [[nodiscard]] nectarState localComputation(
        nectarState s,
        const std::unordered_map<std::string, NeighborData<nectarState, double>>& /*neighborhood*/)
        const override
    {
        constexpr double nectar_regrowth       = 1.0;
        constexpr double nectar_decay          = 0.05;
        constexpr double bee_consumption       = 0.24;
        constexpr double butterfly_consumption = 0.36;
        constexpr double pollen_regrowth       = 1.0;
        constexpr double pollen_decay          = 0.05;
        constexpr double max_nectar            = 100.0;
        constexpr double max_pollen            = 50.0;

        // Nectar dynamics
        if (s.nectar_lvl < max_nectar) {
            s.nectar_lvl += nectar_regrowth * (1.0 + 0.01 * s.pollen_lvl);
        }
        s.nectar_lvl -= nectar_decay          * s.nectar_lvl;
        s.nectar_lvl -= bee_consumption       * s.bees;
        s.nectar_lvl -= butterfly_consumption * s.butterflies;
        s.nectar_lvl  = std::clamp(s.nectar_lvl, 0.0, max_nectar);

        // Pollen dynamics
        s.pollen_lvl -= pollen_decay * s.pollen_lvl;
        if (s.pollen_lvl < max_pollen) {
            s.pollen_lvl += pollen_regrowth * 0.1;
        }
        s.pollen_lvl = std::clamp(s.pollen_lvl, 0.0, max_pollen);

        if (s.pollen_lvl < 0.2) {
            s.pollen_type = 0;
        }

        return s;
    }

    [[nodiscard]] double outputDelay(const nectarState& /*s*/) const override {
        return 1.0;
    }
};

#endif // NECTAR_CELL
