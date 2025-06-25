import glob
import os
import sys
'''
Ce fichier est utilisé par le ppeline de persée pour obtenir l'installeur Pegase le plus récent
'''

list_of_files = glob.glob(f"{sys.argv[1]}\\*.exe")
latest_path = max(list_of_files, key=os.path.getmtime)
latest_file = latest_path.rsplit('\\')[-1]
print(latest_file)