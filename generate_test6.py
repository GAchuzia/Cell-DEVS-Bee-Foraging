import json

SIZE = 10

def is_river(x, y):
    if x == 4 and y % 2 == 0:
        return True
    if x == 5 and y % 2 == 1:
        return True
    return False

def neighbors(x, y):
    return [
        (x-1, y),
        (x+1, y),
        (x, y-1),
        (x, y+1),
        (x, y)  # self
    ]

cells = {}

# Default config (IMPORTANT: add dummy neighborhood for viewer compatibility)
cells["default"] = {
    "delay": "inertial",
    "model": "nectar",
    "state": {
        "nectar": 15.0,
        "pollen": 5.0,
        "bees": 0,
        "butterflies": 0,
        "plant_species": 1,
        "pollen_type": 0,
        "conspecific_pollen": 0.0,
        "heterospecific_pollen": 0.0
    },

    # 🔥 THIS LINE FIXES EVERYTHING
    "neighborhood": [
        { "type": "von_neumann", "range": 1 }
    ]
}

# Generate asymmetric neighborhoods
for x in range(SIZE):
    for y in range(SIZE):
        cell_id = f"({x},{y})"
        cell_neighbors = {}

        for nx, ny in neighbors(x, y):
            if 0 <= nx < SIZE and 0 <= ny < SIZE:

                if is_river(x, y) != is_river(nx, ny):
                    weight = 0.0
                else:
                    weight = 1.0

                neighbor_id = f"({nx},{ny})"
                cell_neighbors[neighbor_id] = weight

        cells[cell_id] = {
            "neighborhood": cell_neighbors
        }

config = {
    "scenario": {
        "shape": [SIZE, SIZE],
        "origin": [0, 0],
        "wrapped": False
    },
    "cells": cells,

    # 👇 THIS ALSO HELPS VIEWER
    "viewer": [
        {
            "field": "nectar",
            "colors": [[255,255,224],[34,139,34]],
            "breaks": [0, 100]
        },
        {
            "field": "pollen",
            "colors": [[255,255,255],[255,165,0]],
            "breaks": [0, 50]
        },
        {
            "field": "bees",
            "colors": [[0,0,0],[255,0,0]],
            "breaks": [0, 50]
        }
    ]
}

with open("test6_water_barrier_config.json", "w") as f:
    json.dump(config, f, indent=2)

print("✅ Fixed test6 config generated!")