#ifndef BUTTERFLY_HPP
#define BUTTERFLY_HPP

#include <algorithm>
#include <string>
#include <vector>

#include <cadmium/modeling/devs/atomic.hpp>

#include "cells/nectar_cell.hpp"

using namespace cadmium;

struct ButterflyState {
    int id;
    std::vector<int> position;
    double consumption_rate;
    double sigma;
    bool should_emit;
};

inline std::ostream& operator<<(std::ostream& os, const ButterflyState& s) {
    os << "<" << s.id << "," << s.position[0] << "," << s.position[1] << ">";
    return os;
}

class Butterfly : public Atomic<ButterflyState> {
public:
    Port<ButterflyMovement> out;
    Port<double> in;

    Butterfly(int id, std::vector<int> pos)
        : Atomic<ButterflyState>("butterfly_" + std::to_string(id), ButterflyState()) {
        out = addOutPort<ButterflyMovement>("out");
        in = addInPort<double>("in");

        state.id = id;
        state.position = std::move(pos);
        state.consumption_rate = 0.35;
        state.sigma = 1.0;
        state.should_emit = true;
    }

    void internalTransition(ButterflyState& state) const override {
        // Faster movers: emulate larger jump by moving two cells each internal transition.
        state.position[0] = (state.position[0] + 2) % 10;
        state.position[1] = (state.position[1] + 2) % 10;
        state.should_emit = true;
        state.sigma = 1.0;
    }

    void externalTransition(ButterflyState& state, double e) const override {
        state.sigma = std::max(0.0, state.sigma - e);
        for (const auto& nectar : in->getBag()) {
            // Butterflies are less selective: keep visiting as long as there is any nectar.
            state.should_emit = nectar > 0.0;
        }
    }

    void output(const ButterflyState& state) const override {
        if (!state.should_emit) return;
        ButterflyMovement msg;
        msg.butterfly_id = state.id;
        msg.x = state.position[0];
        msg.y = state.position[1];
        msg.entering = true;
        msg.consumption_request = state.consumption_rate;
        msg.pollen_type = (state.position[0] < 5) ? 1 : 2;
        out->addMessage(msg);
    }

    [[nodiscard]] double timeAdvance(const ButterflyState& state) const override {
        return state.sigma;
    }
};

#endif
