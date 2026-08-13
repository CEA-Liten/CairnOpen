#!/usr/bin/env python3
"""
update_version_number.py

Update the "Saved with Cairn version" field in a Cairn JSON file.

Usage:
    python update_version_number.py <json_file> <new_version> [-o OUTPUT]

Examples:
    python update_cairn_version.py study.json 6.0.0
    python update_cairn_version.py study.json "6.0.0" -o study_updated.json
"""

from __future__ import annotations

import argparse
import json
import sys
from pathlib import Path

VERSION_KEY = "Saved with Cairn version"
VERSION_KEY_LEGACY = "Saved with PERSEE version"

def load_json(path: Path) -> dict:
    """Read and parse a JSON file, raising a clear error on failure."""
    try:
        with path.open("r", encoding="utf-8") as f:
            return json.load(f)
    except FileNotFoundError:
        sys.exit(f"Error: file not found: {path}")
    except json.JSONDecodeError as e:
        sys.exit(f"Error: '{path}' is not valid JSON ({e})")


def update_version(data: dict, new_version: str) -> str | None:
    """
    Update VERSION_KEY in-place on `data`.
    - If VERSION_KEY exists: use it.
    - Else if VERSION_KEY_LEGACY exists: use it and REMOVE it.
    Returns the old version or None.
    """
    old_version = None
    key_exist = False

    # Prefer modern key
    if VERSION_KEY in data:
        old_version = data[VERSION_KEY]
        key_exist = True

    # Fallback to legacy key, but remove it afterwards
    elif VERSION_KEY_LEGACY in data:
        old_version = data[VERSION_KEY_LEGACY]
        del data[VERSION_KEY_LEGACY]

    # Set new version
    if key_exist:
        data[VERSION_KEY] = str(new_version)
    else:
        # add on top
        new_data = {VERSION_KEY: str(new_version)}
        new_data.update(data)
        data.clear()
        data.update(new_data)        

    return old_version


def write_json(path: Path, data: dict) -> None:
    """Write `data` back to disk as pretty-printed JSON."""
    with path.open("w", encoding="utf-8") as f:
        json.dump(data, f, indent=4, ensure_ascii=False)
        f.write("\n")


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        description=f'Update the "{VERSION_KEY}" field in a Cairn JSON file.'
    )
    parser.add_argument("json_file", type=Path, help="Path to the input JSON file")
    parser.add_argument("new_version", type=str, help="New version string, e.g. 6.0.0")

    out_group = parser.add_mutually_exclusive_group()
    out_group.add_argument(
        "-o", "--output", type=Path, default=None,
        help="Write result to this path instead of the input file (default: <name>_updated.json)",
    )
    return parser.parse_args()


def main() -> None:
    args = parse_args()

    data = load_json(args.json_file)
    old_version = update_version(data, args.new_version)

    if args.output:
        out_path = args.output
    else:
        out_path = args.json_file

    write_json(out_path, data)

    if old_version is None:
        print(f'"{VERSION_KEY}" was not present; added it with value "{args.new_version}".')
    else:
        print(f'"{VERSION_KEY}": "{old_version}" -> "{args.new_version}"')
    print(f"Written to: {out_path}")


if __name__ == "__main__":
    main()