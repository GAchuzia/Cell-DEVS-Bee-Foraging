#ifndef BEE_HPP
#define BEE_HPP

#include <cadmium/modeling/devs/atomic.hpp>
#include <iostream>
#include <string>
#include <vector>
#include <algorithm> 

#include "cells/nectar_cell.hpp"

using namespace cadmium;

// Bee roles 
enum class Role {FORAGER, SCOUT, NURSE};

struct BeeState {
    int id;
    Role role;
    std::vector<int> position; // [x, y]
    double consumption_rate; 
    double nectar_carried;
    double max_cap;
    double sigma;
    bool sending_request; //flag to trigger output 
    bool is_moving;
};

// inline std::ostream& operator<<(std::ostream& os, const BeeState& s) {
//     os << "{"
//        << "\"id\":" << s.id << ","
//        << "\"role\":\"" << (s.role == Role::FORAGER ? "Forager" : (s.role == Role::SCOUT ? "Scout" : "Nurse")) << "\","
//        << "\"pos\":[" << s.position[0] << "," << s.position[1] << "],"
//        << "\"nectar_carried\":" << s.nectar_carried << ","
//        << "\"is_moving\":" << (s.is_moving ? "true" : "false")
//        << "}";
//     return os;
// }

inline std::ostream& operator<<(std::ostream& os, const BeeState& s) {
    os << "<0,0,0>"; 
    return os;
}

class Bee: public Atomic<BeeState> {
public:
    Port<BeeMovement> out; // sends consumption & pollen request
    Port<double> in; // receives nectar levels from cell
    Bee(int id, Role role, std::vector <int> pos) : Atomic<BeeState>("(" + std::to_string(pos[0]) + "," + std::to_string(pos[1]) + ")", BeeState()) {
 
        
        // initialize ports
        out = addOutPort<BeeMovement>("out");
        in  = addInPort<double>("in");

        state.id = id;
        state.role = role;
        state.position = pos;
        state.nectar_carried = 0.0;
        state.is_moving = false;
        state.sigma = 1.0;

        switch(role) {
            case Role::FORAGER:
                state.consumption_rate = 0.5;
                state.max_cap = 10.0;
                break;
            case Role::SCOUT:
                state.consumption_rate = 0.1;
                state.max_cap = 2.0;
                break;
            case Role::NURSE:
                state.consumption_rate = 0.2;
                state.max_cap = 5.0;
                break;
        }

    }

    /* Internal transition: decides where to move next or consume energy */
    void internalTransition(BeeState& state) const override {
        if(state.nectar_carried >= state.max_cap) {
            state.is_moving = true;
        }else {
            state.is_moving = false;
        }
        state.sigma = 1.0;
    }

    /* External transition: receives message from cell about nectar level*/
    void externalTransition(BeeState& state, double e) const override {
        state.sigma -= e;
        for (const auto &msg : in->getBag()) {
            double nectar_in_cell = msg;
            if (state.role == Role::FORAGER && nectar_in_cell > 0){
                double actual_grab = std::min(state.consumption_rate, nectar_in_cell);
                state.nectar_carried += actual_grab;
            }
            if (nectar_in_cell < 1.0) {
                state.is_moving = true;
            }
        }
    }

    /* Bee sends a collect message to grid*/
    void output(const BeeState& state) const override {
        if (!state.is_moving) {
            BeeMovement msg;
            msg.bee_id = state.id;
            msg.x = state.position[0];
            msg.y = state.position[1];
            msg.entering = true;
            msg.consumption_request = state.consumption_rate;
            out->addMessage(msg);
        }
    }

    [[nodiscard]] double timeAdvance(const BeeState& state) const override {
        return state.sigma;
    }
};

#endif