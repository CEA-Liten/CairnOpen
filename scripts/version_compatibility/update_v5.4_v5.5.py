import json
import sys
from cairn import *
from pathlib import Path


def get_param_value(param_list: list, key: str):
    """Return the value of a parameter by key, or None if not found."""
    for param in param_list:
        if param.get("key") == key:
            return param.get("value")
    return None


def set_param_value(param_list: list, key: str, value: str):
    """Set the value of a parameter by key."""
    for param in param_list:
        if param.get("key") == key:
            param["value"] = value
            return
    # Key not found - append it
    param_list.append({"key": key, "value": value})


def convert_components(components: list, context: str) -> int:
    """
    Process a single Components list. Convert every EnergyVector component to either ElectricalCarrier or MaterialCarrier 

    Returns the number of converted components.
    """
    converted = 0

    for comp in components:
        if not isinstance(comp, dict):
            continue
        if comp.get("componentPERSEEType") != "EnergyVector" and comp.get("componentPERSEEType") != "MaterialCarrier":
            continue

        nodeType = comp["nodeType"]

        if nodeType == "EnergyVector":
            comp["nodeType"] = "Material"
            comp["nodeTechnoType"] = "Material"

        option_list = comp.get("optionListJson", [])
        carrierType = get_param_value(option_list, "Type")

        param_list = comp.get("paramListJson", []) 
        potential = get_param_value(param_list, "Potential")

        if carrierType == "Electrical":
            comp["componentPERSEEType"] = "ElectricalCarrier"
            comp["nodeType"] = "Electricity"
            comp["nodeTechnoType"] = "Electricity"
            if potential != None:
                set_param_value(param_list, "Voltage", potential)
        else:
            comp["componentPERSEEType"] = "MaterialCarrier"
            if potential != None:
                set_param_value(param_list, "Pressure", potential)
                set_param_value(param_list, "Temperature", potential)
            if carrierType == "Thermal":
                set_param_value(param_list, "FluxType", "Energy")

        converted += 1
        print(f"  [{context}] Converted '{comp.get('nodeName')}' (id: {comp.get('nodeId')})")

    return converted


def convert_EnergyVector(data: dict) -> tuple[dict, int]:
    """
    Convert EnergyVector components to either ElectricalCarrier or MaterialCarrier in both formats:
      - Pageless: Components list at the root level
      - Paged:    Each page value is a dict containing a Components list

    Returns the modified data and the total count of converted components.
    """
    converted = 0

    # Pageless format: root dict has a "Components" key directly
    if isinstance(data.get("Components"), list):
        converted += convert_components(data["Components"], context="root")
    else:
        # Paged format: iterate over page entries and look for Components inside each
        for page_key, page_value in data.items():
            if not isinstance(page_value, dict):
                continue
            components = page_value.get("Components")
            if not isinstance(components, list):
                continue
            converted += convert_components(components, context=page_key)

    return data, converted

def process_file(json_path: Path) -> int:
    """Load, convert, and overwrite a single JSON file. Returns converted count."""
    with open(json_path, "r", encoding="utf-8") as f:
        data = json.load(f)

    data, count = convert_EnergyVector(data)

    with open(json_path, "w", encoding="utf-8") as f:
        json.dump(data, f, indent='\t', ensure_ascii=False)

    #c = CairnAPI()
    #print(str(json_path))
    #p = c.read_study(str(json_path))
    #p.save_study("")
    
    return count

def main_dir():
    root_dir = Path(sys.argv[1])
    if not root_dir.is_dir():
        print(f"Error: '{root_dir}' is not a directory.")
        sys.exit(1)

    json_files = sorted(root_dir.rglob("*.json"))
    if not json_files:
        print(f"No JSON files found in '{root_dir}'.")
        sys.exit(0)

    print(f"Found {len(json_files)} JSON file(s) in '{root_dir}'\n")

    total_files_changed = 0
    total_converted = 0

    for json_path in json_files:
        try:
            count = process_file(json_path)
            total_converted += count
            if count:
                total_files_changed += 1
                print(f"[CHANGED] {json_path} ({count} component(s) converted)")
            else:
                print(f"[unchanged] {json_path}")
        except (json.JSONDecodeError, OSError) as e:
            print(f"[ERROR] {json_path} - {e}")

    print(f"\nDone. {total_files_changed}/{len(json_files)} file(s) modified, "
          f"{total_converted} component(s) converted in total.")

def main():
    input_path = Path(sys.argv[1])
    output_path = Path(sys.argv[2]) if len(sys.argv) >= 3 else input_path.with_stem(input_path.stem)

    print(f"Reading: {input_path}")
    with open(input_path, "r", encoding="utf-8") as f:
        data = json.load(f)

    print("Processing components...")
    data, count = convert_EnergyVector(data)

    print(f"\nTotal converted: {count} component(s)")

    with open(output_path, "w", encoding="utf-8") as f:
        json.dump(data, f, indent='\t', ensure_ascii=False)
   
    print(f"Output written to: {output_path}")


if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Usage: python update_v5.4_v5.5.py <input.json> [output.json]")
        print("Or:    python update_v5.4_v5.5.py <directory>")
        sys.exit(1)

    arg1 = Path(sys.argv[1])
    if not arg1.is_dir():
        main()
    else:
        main_dir()
