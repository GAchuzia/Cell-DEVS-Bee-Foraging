import json

SIZE = 10

def all_neighbors(x, y):
    return [
        (x - 1, y),
        (x + 1, y),
        (x, y - 1),
        (x, y + 1),
        (x, y)  # self
    ]

def crosses_half_river(x, y, nx, ny):
    # Block only vertical crossings between row 4 and row 5
    # for columns x = 0..4
    if x != nx:
        return False
    if x < 0 or x > 4:
        return False
    return {y, ny} == {4, 5}

cells = {}

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
    "neighborhood": [
        { "type": "von_neumann", "range": 1 }
    ]
}

for x in range(SIZE):
    for y in range(SIZE):
        cell_id = f"({x},{y})"
        cell_neighbors = {}

        for nx, ny in all_neighbors(x, y):
            if 0 <= nx < SIZE and 0 <= ny < SIZE:
                if crosses_half_river(x, y, nx, ny):
                    continue
                neighbor_id = f"({nx},{ny})"
                cell_neighbors[neighbor_id] = 1.0

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
    "viewer": [
        {
            "field": "nectar",
            "colors": [
                [255, 255, 224],
                [220, 245, 200],
                [180, 230, 170],
                [120, 200, 120],
                [34, 139, 34]
            ],
            "breaks": [0, 5, 10, 15, 20, 25]
        },
        {
            "field": "pollen",
            "colors": [
                [255, 255, 255],
                [255, 235, 180],
                [255, 200, 90],
                [255, 165, 0],
                [220, 110, 0]
            ],
            "breaks": [0, 1, 2, 4, 6, 10]
        },
        {
            "field": "butterflies",
            "colors": [
                [0, 0, 0],
                [80, 0, 120],
                [140, 0, 200],
                [220, 80, 255]
            ],
            "breaks": [0, 1, 2, 3, 5]
        }
    ]
}

with open("config/tests/test6_water_barrier_config.json", "w") as f:
    json.dump(config, f, indent=2)

print("Generated config/tests/test6_water_barrier_config.json")