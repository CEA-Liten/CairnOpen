# -*- coding: utf-8 -*-
"""
Rename Compressor MassFlowRate ports to InMassFlowRate / OutMassFlowRate
"""

import argparse
import json
from pathlib import Path


def fix_compressor_ports(component: dict) -> int:
    """
    Given a single component dict, rename any port whose variable is
    "MassFlowRate" to "InMassFlowRate" (INPUT) or "OutMassFlowRate" (OUTPUT).
    Returns the number of ports changed.
    """
    changed = 0
    for port_group in component.get("nodePortsData", []):
        for port in port_group.get("ports", []):
            direction = port.get("direction")
            variable = port.get("variable")
            if direction == "INPUT" and variable == "MassFlowRate":
                port["variable"] = "InMassFlowRate"
                changed += 1
            elif direction == "OUTPUT" and variable == "MassFlowRate":
                port["variable"] = "OutMassFlowRate"
                changed += 1
    return changed


def convert_components(components: list, context: str = "root") -> int:
    """
    Iterate over a list of component dicts and fix any Compressor ports found.
    Returns the number of ports changed.
    """
    converted = 0
    for comp in components:
        if not isinstance(comp, dict):
            continue
        if comp.get("nodeType") == "Compressor" or comp.get("nodeTechnoType") == "Compressor":
            n = fix_compressor_ports(comp)
            if n:
                print(f'  [{context}] Compressor "{comp.get("nodeName", comp.get("nodeId"))}": '
                      f'{n} port(s) renamed')
            converted += n
    return converted


def convert_study(data: dict) -> int:
    """
    Handle both JSON layouts:
      - root format: data["Components"] is a list
      - paged format: data is a dict of pages, each possibly containing "Components"
    Returns the total number of ports changed.
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
        description="Rename Compressor MassFlowRate ports to InMassFlowRate/OutMassFlowRate"
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

    print(f"Done. {total_changed} port(s) renamed. Saved to: {json_file}")


if __name__ == "__main__":
    main()
