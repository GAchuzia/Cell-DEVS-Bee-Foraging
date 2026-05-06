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
    std::vector<int> previous_position;
    double consumption_rate;
    double sigma;
    bool should_emit;
    bool just_moved;
};


inline std::ostream& operator<<(std::ostream& os, const ButterflyState& s) {
    os << "<" << s.position[0] << "," << s.position[1] << "," << s.consumption_rate << ">";
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
        state.position = pos;
        state.previous_position = pos;
        state.consumption_rate = 0.35;
        state.just_moved = false; 
        state.sigma = 1.0;
        state.should_emit = true;
    }

    void internalTransition(ButterflyState& state) const override {
        // Faster movers: emulate larger jump by moving two cells each internal transition.
        state.previous_position = state.position;
        state.position[0] = (state.position[0] + 2) % 10;
        state.position[1] = (state.position[1] + 2) % 10;
        state.should_emit = true;
        state.just_moved = true;
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

        // Send leaving message to old cell
        if (state.just_moved) {
            ButterflyMovement exit_msg;
            exit_msg.butterfly_id = state.id;
            exit_msg.x = state.previous_position[0];
            exit_msg.y = state.previous_position[1];
            exit_msg.action = 0; // Leaving
            exit_msg.consumption_request = 0.0;
            exit_msg.pollen_type = 0;
            out->addMessage(exit_msg);
        }
        // Send arriving/staying message to new cell
        ButterflyMovement enter_msg;
        enter_msg.butterfly_id = state.id;
        enter_msg.x = state.position[0];
        enter_msg.y = state.position[1];
        enter_msg.action = state.just_moved ? 1 : 2; // Arriving if moved, otherwise staying
        enter_msg.consumption_request = state.consumption_rate;
        enter_msg.pollen_type = (state.position[0] < 5) ? 1 : 2;
        out->addMessage(enter_msg);
    }

    [[nodiscard]] double timeAdvance(const ButterflyState& state) const override {
        return state.sigma;
    }
};

#endif
