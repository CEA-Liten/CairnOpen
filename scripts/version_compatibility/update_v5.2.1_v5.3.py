# -*- coding: utf-8 -*-

import os
import sys
import re

def replace_key_opex(input_file, output_file):
    try:
        with open(input_file, 'r', encoding='utf-8') as f:
            content = f.read()

        # Replace ONLY: "key": "Opex"
        pattern = r'"key"\s*:\s*"Opex"'
        replacement = '"key": "FixedOpex"'

        content = re.sub(pattern, replacement, content)

        # Replace ONLY: "key": "OpexOffset"
        pattern = r'"key"\s*:\s*"OpexOffset"'
        replacement = '"key": "FixedOpexOffset"'

        content = re.sub(pattern, replacement, content)

        with open(output_file, 'w', encoding='utf-8') as f:
            f.write(content)

    except Exception as e:
        print(f"Error processing '{input_file}': {e}")


def update_v521_v53(directory, file_type):
    for root, _, files in os.walk(directory):
        for file in files:
            if file.endswith(file_type):
                json_file = os.path.join(root, file)
                replace_key_opex(json_file, json_file)

def main():
    if len(sys.argv) < 2:
        print("Usage:")
        print("  Single file mode:")
        print("     python update_v521_v53.py <file.json>")
        print("")
        print("  Directory mode:")
        print("     python update_v521_v53.py <directory> <file_extension>")
        print("     Example: python update_v521_v53.py C:\\path\\to\\models .json")
        sys.exit(1)

    # --- Single-file mode ---
    if len(sys.argv) == 2:
        file_path = sys.argv[1]

        if not os.path.isfile(file_path):
            print(f"Error: '{file_path}' is not a valid file.")
            sys.exit(1)

        print(f"Updating single file: {file_path}")
        replace_key_opex(file_path, file_path)
        print("Done.")
        return

    # --- Directory mode ---
    if len(sys.argv) == 3:
        directory = sys.argv[1]
        file_type = sys.argv[2]

        if not os.path.isdir(directory):
            print(f"Error: '{directory}' is not a valid directory.")
            sys.exit(1)

        print(f"Updating directory: {directory}")
        print(f"File type: {file_type}")
        update_v521_v53(directory, file_type)
        print("Done.")
        return

if __name__ == "__main__":
    main()
