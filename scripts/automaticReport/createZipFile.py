
"""
Created on 19/02/2026

@author: Ali KASSEM
"""

import zipfile
import os
import sys

def createZip(dir: str, filename_list: list[str], name: str) -> str:
    """
    Creates a zip file from a list of files in a given directory.
    
    :param dir: The directory where the files are located.
    :param filename_list: List of filenames (relative to dir) to include in the zip.
    :param name: Zip file name or path. If relative/filename, it's saved relative to dir.
    :return: The absolute path of the created zip file.
    """
    # Resolve zip output path
    if os.path.isabs(name):
        zip_path = name
    else:
        zip_path = os.path.join(dir, name)

    # Ensure .zip extension
    if not zip_path.endswith(".zip"):
        zip_path += ".zip"

    missing_files = []

    with zipfile.ZipFile(zip_path, 'w', zipfile.ZIP_DEFLATED) as zf:
        for filename in filename_list:
            abs_path = os.path.join(dir, filename)
            if not os.path.isfile(abs_path):
                print(f"Warning: file not found, skipping: {abs_path}")
                missing_files.append(filename)
                continue
            zf.write(abs_path, os.path.basename(filename))
            print(f"Added: {filename}")

    if missing_files:
        print(f"Zip created with {len(missing_files)} missing file(s): {zip_path}")
    else:
        print(f"Zip created successfully: {zip_path}")

    return zip_path

if __name__ == '__main__':

    if len(sys.argv) > 3:
        zipName = sys.argv[1]
        dir_path = sys.argv[2]

        filename_list = [] 
        for i in range(3, len(sys.argv)):
            if sys.argv[i] != "":
                filename_list.append(sys.argv[i])
            

        createZip(dir_path, filename_list, zipName)
