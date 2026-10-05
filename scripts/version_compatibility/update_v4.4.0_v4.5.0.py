# -*- coding: utf-8 -*-
"""
Copy the "NomPower" setting value into "MaxPower" for ElectrolyzerDetailed
and Electrolyzer components.
"""
import argparse
import json
from pathlib import Path

TARGET_NODE_TYPES = ("ElectrolyzerDetailed", "Electrolyzer")
SOURCE_KEY = "NomPower"
TARGET_KEY = "MaxPower"


def fix_electrolyzer_settings(component: dict) -> bool:
    """
    In a single component's paramListJson, copy the value of SOURCE_KEY
    into TARGET_KEY. TARGET_KEY is added to paramListJson if it doesn't
    already exist (and paramListJson itself is created if missing).
    Returns True if a change was made.
    """
    params = component.get("paramListJson")
    if not isinstance(params, list):
        params = []
        component["paramListJson"] = params

    nom_pow = None
    target_entry = None
    for param in params:
        if not isinstance(param, dict):
            continue
        if param.get("key") == SOURCE_KEY:
            nom_pow = param.get("value")
        elif param.get("key") == TARGET_KEY:
            target_entry = param

    if nom_pow is None:
        return False  # no NomPower setting found, nothing to copy

    if target_entry is None:
        # TARGET_KEY doesn't exist yet -> add it
        params.append({"key": TARGET_KEY, "value": nom_pow})
        return True

    if target_entry.get("value") == nom_pow:
        return False  # already up to date

    target_entry["value"] = nom_pow
    return True


def convert_components(components: list, context: str = "root") -> int:
    """
    Iterate over a list of component dicts and fix any matching
    Electrolyzer(Detailed) components found. Returns count changed.
    """
    converted = 0
    for comp in components:
        if not isinstance(comp, dict):
            continue
        node_type = comp.get("nodeType")
        techno_type = comp.get("nodeTechnoType")
        if node_type in TARGET_NODE_TYPES or techno_type in TARGET_NODE_TYPES:
            if fix_electrolyzer_settings(comp):
                print(f'  [{context}] {node_type or techno_type} '
                      f'"{comp.get("nodeName", comp.get("nodeId"))}": '
                      f'{TARGET_KEY} set from {SOURCE_KEY}')
                converted += 1
    return converted


def convert_study(data: dict) -> int:
    """
    Handle both JSON layouts:
      - root format: data["Components"] is a list
      - paged format: data is a dict of pages, each possibly containing "Components"
    Returns the total number of components changed.
    """
    converted = 0
    if isinstance(data.get("Components"), list):
        converted += convert_components(data["Components"], context="root")
    else:
        # Paged format: iterate over page entries and look for Components inside each
        for page_key, page_value in data.items():
            if not isinstance(page_value, dict):
                continue
            components = page_value.get("Components")
            if isinstance(components, list):
                converted += convert_components(components, context=page_key)
    return converted


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        description=f'Copy "{SOURCE_KEY}" into "{TARGET_KEY}" for '
                    f'{", ".join(TARGET_NODE_TYPES)} components in a Cairn study JSON file '
                    f'(edited in place).'
    )
    parser.add_argument("json_file", type=Path, help="Path to the Cairn study JSON file")
    return parser.parse_args()


def main() -> None:
    args = parse_args()
    json_file = args.json_file

    with json_file.open("r", encoding="utf-8") as f:
        data = json.load(f)

    total_changed = convert_study(data)

    with json_file.open("w", encoding="utf-8") as f:
        json.dump(data, f, indent=4, ensure_ascii=False)
        f.write("\n")

    print(f"Done. {total_changed} component(s) updated. Saved to: {json_file}")


if __name__ == "__main__":
    main()