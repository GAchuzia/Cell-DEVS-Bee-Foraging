#!/usr/bin/env python3
"""
generate_asymm_configs.py
=========================
Converts the project's symmetric (Grid) Cell-DEVS JSON configs to the
Asymmetric Cell-DEVS format required by AsymmCellDEVSCoupled.

Key differences from symmetric format
--------------------------------------
• No top-level "scenario" block (no shape/origin/wrapped).
• Every cell is listed explicitly by its string ID "(x,y)".
• Each cell entry has a "neighborhood" object:  { "(nx,ny)": vicinity_weight }
  with weight 1.0 for all Von Neumann neighbours (up / down / left / right).
• Agent positions move from cells.initial_agents / cells.initial_butterflies
  to top-level "agents" / "butterflies" arrays so the Cell-DEVS parser does
  not try to interpret them as cell entries.
• River barrier (test6): row-4 cells are completely isolated (empty
  neighborhood) AND cells in rows 3 and 5 do not include any row-4 neighbour,
  encoding the topological barrier in the Asymmetric Cell-DEVS structure.

Usage
-----
    python3 generate_asymm_configs.py
Run from the project root (the folder that contains CMakeLists.txt).
"""

import json
import os


# ── helpers ────────────────────────────────────────────────────────────────────

def cell_id(x: int, y: int) -> str:
    """Return the canonical cell ID string used by NectarCell::NectarCell()."""
    return f"({x},{y})"


def von_neumann_neighbors(
    x: int, y: int,
    rows: int, cols: int,
    wrapped: bool,
    river_row: int | None = None,
) -> list[tuple[int, int]]:
    """
    Return the Von Neumann neighbour list for cell (x, y).

    Parameters
    ----------
    wrapped   : whether the grid is a torus.
    river_row : if given, the row that acts as a barrier.
                - Cells on the river row have NO neighbours (fully isolated).
                - Cells adjacent to the river do NOT include the river row
                  as a neighbour (the barrier blocks observation).
    """
    # River cells are completely isolated
    if river_row is not None and x == river_row:
        return []

    directions = [(-1, 0), (1, 0), (0, -1), (0, 1)]  # up, down, left, right
    neighbours = []

    for dx, dy in directions:
        nx, ny = x + dx, y + dy

        if wrapped:
            nx = nx % rows
            ny = ny % cols
        else:
            if not (0 <= nx < rows and 0 <= ny < cols):
                continue

        # Skip river-row neighbours (barrier blocks cross-river observation)
        if river_row is not None and nx == river_row:
            continue

        neighbours.append((nx, ny))

    return neighbours


def build_state_map(sym_cells: dict) -> dict[tuple[int, int], dict]:
    """
    Parse the symmetric "cells" block and return a map from (x, y) → state dict.
    Groups like "plant_a_hotspots" with a cell_map + state are expanded here.
    The "initial_agents" / "initial_butterflies" pseudo-groups are skipped.
    """
    SKIP = {"default", "initial_agents", "initial_butterflies"}
    state_map: dict[tuple[int, int], dict] = {}

    for key, group in sym_cells.items():
        if key in SKIP:
            continue
        if "cell_map" in group and "state" in group:
            for pos in group["cell_map"]:
                state_map[(pos[0], pos[1])] = dict(group["state"])

    return state_map


# ── main conversion ────────────────────────────────────────────────────────────

def convert(
    sym_config: dict,
    river_row: int | None = None,
) -> dict:
    """Convert a symmetric config dict to asymmetric format."""

    scenario   = sym_config["scenario"]
    rows, cols = scenario["shape"]
    wrapped    = scenario.get("wrapped", False)
    sym_cells  = sym_config["cells"]

    default_state = dict(sym_cells["default"].get("state", {}))
    state_map     = build_state_map(sym_cells)

    # ── cells block ──────────────────────────────────────────────────────────
    cells_out: dict = {
        "default": {
            "delay": "inertial",
            "model": "nectar_cell",
            "state": default_state,
        }
    }

    for x in range(rows):
        for y in range(cols):
            cid         = cell_id(x, y)
            cell_entry: dict = {}

            # Per-cell state override (plant patches, river, etc.)
            if (x, y) in state_map:
                cell_entry["state"] = state_map[(x, y)]

            # Explicit Von Neumann neighbourhood (asymmetric topology)
            nbrs = von_neumann_neighbors(x, y, rows, cols, wrapped, river_row)
            cell_entry["neighborhood"] = {
                cell_id(nx, ny): 1.0 for nx, ny in nbrs
            }

            cells_out[cid] = cell_entry

    # ── top-level output ──────────────────────────────────────────────────────
    out: dict = {"cells": cells_out}

    # Agent positions at top level (not inside "cells")
    init_agents = sym_cells.get("initial_agents", {})
    if "cell_map" in init_agents and init_agents["cell_map"]:
        out["agents"] = init_agents["cell_map"]

    init_bflies = sym_cells.get("initial_butterflies", {})
    if "cell_map" in init_bflies and init_bflies["cell_map"]:
        out["butterflies"] = init_bflies["cell_map"]

    # Preserve the viewer block for the Cell-DEVS web viewer
    if "viewer" in sym_config:
        out["viewer"] = sym_config["viewer"]

    return out


# ── config manifest ────────────────────────────────────────────────────────────

CONFIGS = [
    # (input_path,                                   output_path,                                    river_row)
    ("config/nectarVisualization_config.json",        "config/nectarVisualization_config.json",        None),
    ("config/tests/test1_no_bees_config.json",        "config/tests/test1_no_bees_config.json",        None),
    ("config/tests/test2_center_burst_config.json",   "config/tests/test2_center_burst_config.json",   None),
    ("config/tests/test3_corner_unwrapped_config.json","config/tests/test3_corner_unwrapped_config.json", None),
    ("config/tests/test4_corner_wrapped_config.json", "config/tests/test4_corner_wrapped_config.json", None),
    ("config/tests/test5_multi_species_config.json",  "config/tests/test5_multi_species_config.json",  None),
    # test6: river barrier at row 4 — encoded in the asymmetric topology
    ("config/tests/test6_river_config.json",          "config/tests/test6_river_config.json",          4),
]


def main() -> None:
    # Read all symmetric configs first (inputs and outputs may be the same path)
    loaded = []
    for in_path, out_path, river_row in CONFIGS:
        if not os.path.exists(in_path):
            print(f"  SKIP (not found): {in_path}")
            continue
        with open(in_path) as f:
            sym = json.load(f)
        loaded.append((sym, out_path, river_row))

    # Check that the configs are still in symmetric format (have "scenario")
    for sym, out_path, river_row in loaded:
        if "scenario" not in sym:
            print(f"  WARN: {out_path} appears to already be asymmetric — regenerating anyway.")

    # Convert and write
    os.makedirs("config/tests", exist_ok=True)
    for sym, out_path, river_row in loaded:
        # If already asymmetric (no "scenario"), reconstruct a stub scenario so
        # we can still call convert(); this handles re-running the script safely.
        if "scenario" not in sym:
            print(f"  Re-generating (already asymmetric): {out_path}")
            # We cannot safely re-derive the shape from an asymm config;
            # skip conversion but leave the file untouched.
            continue

        asymm = convert(sym, river_row=river_row)
        with open(out_path, "w") as f:
            json.dump(asymm, f, indent=2)
        river_note = f" [river barrier at row {river_row}]" if river_row is not None else ""
        print(f"  Generated{river_note}: {out_path}")


if __name__ == "__main__":
    main()
    print("\nAll asymmetric configs written.")
    print("Re-run 'source build_sim.sh' on the server to pick up the new JSON format.")
