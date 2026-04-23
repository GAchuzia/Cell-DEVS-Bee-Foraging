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
enum class Role {FORAGER, SCOUT};

struct BeeState {
    int id;
    Role role;
    std::vector<int> position; // [x, y]
    std::vector<int> prev_position; // [x, y]
    double consumption_rate; 
    double nectar_carried;
    double max_cap;
    double sigma;
    bool just_moved; //flag to trigger output 
    bool is_moving;
};

inline std::ostream& operator<<(std::ostream& os, const BeeState& s) {
    os << "<" << s.position[0] << "," << s.position[1] << "," << s.nectar_carried << ">"; 
    return os;
}

class Bee: public Atomic<BeeState> {
public:
    Port<BeeMovement> out; // sends consumption & pollen request
    Port<double> in; // receives nectar levels from cell
    Bee(int id, Role role, std::vector <int> pos) :
     Atomic<BeeState>("(" + std::to_string(pos[0]) + "," + std::to_string(pos[1]) + ")", BeeState()) {
        // initialize ports
        out = addOutPort<BeeMovement>("out");
        in  = addInPort<double>("in");

        state.id = id;
        state.role = role;
        state.position = pos;
        state.prev_position = pos;

        state.nectar_carried = 0.0;
        state.is_moving = false;
        state.just_moved = false; 
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
        }
    }

    /* Internal transition: decides where to move next or consume energy */
    void internalTransition(BeeState& state) const override {
        if(state.nectar_carried >= state.max_cap || state.is_moving) {
            state.prev_position = state.position;
            state.position[0] = (state.position[0] + 1) % 10;
            // state.position[1] = (state.position[1] + 1) % 10;
            
            state.nectar_carried = 0.0; // Reset capacity after moving
            state.is_moving = false;
            state.just_moved = true;

        }else {
            // state.is_moving = false;
            state.just_moved = false;
        }
        state.sigma = 1.0;
    }

    /* External transition: receives message from cell about nectar level*/
    void externalTransition(BeeState& state, double e) const override {
        state.sigma -= e;
        for (const auto &msg : in->getBag()) {
            double nectar_in_cell = msg;
            if (state.role == Role::FORAGER && nectar_in_cell > 0.1){
                double actual_grab = std::min(state.consumption_rate, nectar_in_cell);
                state.nectar_carried += actual_grab;
                state.is_moving = false; // Stay and eat
            }
            if (nectar_in_cell < 1.0) {
                state.is_moving = true;
            }
        }
    }

    /* Bee sends a collect message to grid*/
    void output(const BeeState& state) const override {
        if (state.just_moved) {
            // Send leaving message to old  cell
            BeeMovement exit_msg;
            exit_msg.bee_id = state.id;
            exit_msg.x = state.prev_position[0];
            exit_msg.y = state.prev_position[1];
            exit_msg.action = 0; // Leaving
            exit_msg.consumption_request = 0.0;
            exit_msg.pollen_type = 0;
            out->addMessage(exit_msg);

            // Send arriving message to new cell
            BeeMovement enter_msg;
            enter_msg.bee_id = state.id;
            enter_msg.x = state.position[0];
            enter_msg.y = state.position[1];
            enter_msg.action = 1; // Arriving
            enter_msg.consumption_request = 0.0; // Don't eat on the exact tick of arrival
            enter_msg.pollen_type = (state.position[0] < 5) ? 1 : 2;
            out->addMessage(enter_msg);

        } else {
            // staying 
            BeeMovement stay_msg;
            stay_msg.bee_id = state.id;
            stay_msg.x = state.position[0];
            stay_msg.y = state.position[1];
            stay_msg.action = 2; // Staying
            stay_msg.consumption_request = state.consumption_rate;
            stay_msg.pollen_type = (state.position[0] < 5) ? 1 : 2;
            out->addMessage(stay_msg);
        }
    }

    [[nodiscard]] double timeAdvance(const BeeState& state) const override {
        return state.sigma;
    }
};

#endif