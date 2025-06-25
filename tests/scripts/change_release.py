import os
import sys
import re
from subprocess import Popen, PIPE
import argparse

def get_new_version(file, name, pos):

    if "persee" in name:

        with open(file, 'r') as pfile:
            new_file = ""

            for line in pfile:
                #print(line)

                if "static QString Persee_Release(" in line:
                    #version = re.search("static QString Persee_Release(\" Persee Release : (.*) - \") ;", line)
                    version = line[line.find("Release : ")+len("Release : "):line.rfind(" - \"")]
                    splitted_version = version.split(".")
                    splitted_version[pos] = str(int(splitted_version[pos]) + 1)
                    new_version = ".".join(splitted_version)

                    line = line.replace(version, new_version)
                
                new_file += f"{line}"

            with open(file, 'w') as npfile:
                npfile.write(new_file)
        

    elif "mipm" in name:

         with open(file, 'r') as mfile:
             new_file = ""

             for line in mfile:

                 if "static std::string MIPModeler_Release" in line:
                    version = line[line.find("Release(\"")+len("Release(\""):line.rfind("\") ;")]
                    splitted_version = version.split(".")
                    splitted_version[pos] = str(int(splitted_version[pos]) + 1)
                    new_version = ".".join(splitted_version)

                    line = line.replace(version, new_version)

                 new_file += f"{line}"

             with open(file, 'w') as npfile:
                npfile.write(new_file)
    
    elif "gui" in name:
        with open(file, 'r') as mfile:
            new_file = ""

            for line in mfile:
                if pos == 2:
                    if "PATCH_VERSION" in line:
                        version = line[line.find("PATCH_VERSION ")+len("PATCH_VERSION "):line.rfind(")")]
                        line = line.replace(version, str(int(version) + 1))
                if pos == 1:
                    if "MINOR_VERSION" in line:
                        version = line[line.find("MINOR_VERSION ")+len("MINOR_VERSION "):line.rfind(")")]
                        line = line.replace(version, str(int(version) + 1))
                new_file += f"{line}"
            with open(file, 'w') as npfile:
                npfile.write(new_file)
                
    else:
        return None


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--product', dest='product', type=str, help='Add product (persee or pegase)', default='persee')
    args = parser.parse_args()
    print (args.product)
    
    
    cmd = "git rev-parse --abbrev-ref HEAD"

    process = Popen(cmd, shell=True,
                           stdout=PIPE, 
                           stderr=PIPE)

    out, err = process.communicate()

    print(out.decode("utf-8"))

    branche = out.decode("utf-8")
    
    if args.product=='persee':
        fileVersion = "lib/export/Persee/cmake/ProjectVersion.cmake"
    else:
        fileVersion = "cmake/ProjectVersion.cmake"

    if "Integration" in branche or "integration" in branche:
        vgui = get_new_version(fileVersion, "gui", 2)

        print(vgui)
        return
    elif "master" in branche:
        vgui = get_new_version(fileVersion, "gui", 1)
        return
    return


if __name__ == "__main__":    
    main()