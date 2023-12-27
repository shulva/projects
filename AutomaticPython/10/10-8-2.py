#!/usr/bin/env python
# pdf-285 book-196
# input a folder destination to collect the directory of the document which is bigger than 100mb
# the best is your pdf directory

import os,sys
from pathlib import Path

arg_dest = str(sys.argv[1])
path_dest = Path(arg_dest)
path_cwd = Path.cwd()

for folderName,subfolders,filenames in os.walk(arg_dest):
    path_folder = Path(folderName)

    for filename in filenames:
        if os.path.getsize(os.path.join(path_folder,filename))>1024000 :
            print(os.path.join(path_folder,filename))

