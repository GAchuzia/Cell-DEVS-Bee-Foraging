#ifndef NECTAR_STATE
#define NECTAR_STATE

#include <iostream>
#include <nlohmann/json.hpp>

struct nectarState {
    double nectar_lvl;
    double pollen_lvl;
    int bees;
    int butterflies;
    int plant_species;  // 0 = none, 1 = Plant A, 2 = Plant B
    int pollen_type;    // 0 = none, 1 = Plant A, 2 = Plant B
    double conspecific_pollen;
    double heterospecific_pollen;

    // Default constructor
    nectarState()
        : nectar_lvl(10.0),
          pollen_lvl(5.0),
          bees(0),
          butterflies(0),
          plant_species(1),
          pollen_type(0),
          conspecific_pollen(0.0),
          heterospecific_pollen(0.0) {}
};


inline std::ostream& operator<<(std::ostream& os, const nectarState& s) {
    os << "<" << s.nectar_lvl << "," << s.pollen_lvl << "," << s.bees << ","
       << s.butterflies << "," << s.plant_species << "," << s.pollen_type
       << "," << s.conspecific_pollen << "," << s.heterospecific_pollen << ">";
    return os;
}

inline bool operator==(const nectarState& a, const nectarState& b) {
    return a.nectar_lvl == b.nectar_lvl &&
           a.pollen_lvl == b.pollen_lvl &&
           a.bees == b.bees &&
           a.butterflies == b.butterflies &&
           a.plant_species == b.plant_species &&
           a.pollen_type == b.pollen_type &&
           a.conspecific_pollen == b.conspecific_pollen &&
           a.heterospecific_pollen == b.heterospecific_pollen;
}

inline bool operator!=(const nectarState& a, const nectarState& b) {
    return a.nectar_lvl != b.nectar_lvl ||
           a.pollen_lvl != b.pollen_lvl ||
           a.bees != b.bees ||
           a.butterflies != b.butterflies ||
           a.plant_species != b.plant_species ||
           a.pollen_type != b.pollen_type ||
           a.conspecific_pollen != b.conspecific_pollen ||
           a.heterospecific_pollen != b.heterospecific_pollen;
}

inline void to_json(nlohmann::json& j, const nectarState& s) {
    j = nlohmann::json{
        {"nectar", s.nectar_lvl}, 
        {"pollen", s.pollen_lvl}, 
        {"bees", s.bees},
        {"butterflies", s.butterflies},
        {"plant_species", s.plant_species},
        {"pollen_type", s.pollen_type},
        {"conspecific_pollen", s.conspecific_pollen},
        {"heterospecific_pollen", s.heterospecific_pollen}
    };
}

// Parses initial values from JSON config file
inline void from_json(const nlohmann::json& j, nectarState& s) {
    s.nectar_lvl = j.value("nectar", 10.0);
    s.pollen_lvl = j.value("pollen", 5.0);
    s.bees = j.value("bees", 0);
    s.butterflies = j.value("butterflies", 0);
    s.plant_species = j.value("plant_species", 1);
    s.pollen_type = j.value("pollen_type", 0);
    s.conspecific_pollen = j.value("conspecific_pollen", 0.0);
    s.heterospecific_pollen = j.value("heterospecific_pollen", 0.0);
}

#endif 