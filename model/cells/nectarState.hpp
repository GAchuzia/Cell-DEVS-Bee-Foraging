#ifndef NECTAR_STATE
#define NECTAR_STATE

#include <iostream>
#include <nlohmann/json.hpp>

struct nectarState {
    double nectar_lvl;
    double pollen_lvl;
    int bees;

    // Default constructor
    nectarState() : nectar_lvl(10.0), pollen_lvl(5.0), bees(0) {}
};


inline std::ostream& operator<<(std::ostream& os, const nectarState& s) {
    os << "<" << s.nectar_lvl << "," << s.pollen_lvl << "," << s.bees << ">";
    return os;
}

inline bool operator!=(const nectarState& a, const nectarState& b) {
    return a.nectar_lvl != b.nectar_lvl ||
           a.pollen_lvl != b.pollen_lvl ||
           a.bees != b.bees;
}

// Parses initial values from JSON config file
inline void from_json(const nlohmann::json& j, nectarState& s) {
    j.at("nectar").get_to(s.nectar_lvl);
    j.at("pollen").get_to(s.pollen_lvl);
    j.at("bees").get_to(s.bees);
}

#endif 