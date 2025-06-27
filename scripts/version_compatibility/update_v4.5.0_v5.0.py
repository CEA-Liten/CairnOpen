# -*- coding: utf-8 -*-
"""
Created on June 2025

@author: pp265749
"""
import os
import shutil


def replace_string_in_file(input_file, output_file, string_to_replace, new_string):
    try:
        # Open the input file in read mode
        with open(input_file, 'r', encoding='utf-8') as file:
            content = file.read()

        # Replace the string
        new_content = content.replace(string_to_replace, new_string)

        # Open the output file in write mode
        with open(output_file, 'w', encoding='utf-8') as file:
            file.write(new_content)

    except FileNotFoundError:
        print(f"The file '{input_file}' was not found.")
    except Exception as e:
        print(f"The file '{input_file}:An error occurred: {e}")

def update_v450_v50(directory, file_type):
    for root, _, files in os.walk(directory):
        for file in files:
            if file.endswith(file_type):
                json_file = os.path.join(root, file)
                replace_string_in_file(json_file, json_file, "RampModel", "Ramp")
                replace_string_in_file(json_file, json_file, "MultiObjective", "ManualObjective")


update_v450_v50("C:\\Users\\PP265749\\git\\cairnopen\\tests",".json")


update_v450_v50("C:\\Users\\PP265749\\git\\cairnopen\\tests",".csv")