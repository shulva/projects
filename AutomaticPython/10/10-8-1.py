#!/usr/bin/env python
# pdf-285 book-196

from pathlib import Path
import os,sys,shutil

arg_dest = str(sys.argv[1])
path_dest = Path(arg_dest)
path_cwd = Path.cwd()

for folderName,subfolders,filenames in os.walk(arg_dest):

    path_folder = Path(folderName)
    os.makedirs(path_cwd/"10-8-1")

    for filename in list(path_folder.glob('*.txt')):
        shutil.copy(path_folder/filename,path_cwd/"10-8-1")
